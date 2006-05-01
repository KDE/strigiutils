#include "archiveengine.h"
#include "archivedirengine.h"
#include "zipinputstream.h"
#include "tarinputstream.h"
#include "streamengine.h"
#include "subinputstream.h"
#include "bz2inputstream.h"
#include "gzipinputstream.h"
#include <QtCore/QFSFileEngine>
#include <QtDebug>
using namespace jstreams;

FileEntry::~FileEntry() {
    QHash<const QString, FileEntry*>::const_iterator i = entries.constBegin();
    while (i != entries.constEnd()) {
        delete i.value();
        ++i;
    }
}
FileEntry*
FileEntry::add(const QString &name) {
//    printf("adding '%s' to '%s'\n", (const char*)name.toUtf8(), (const char*)this->name.toUtf8());
    FileEntry *fe = new FileEntry(name);
    entries.insert(name, fe);
    return fe;
}
FileEntry *
FileEntry::getEntry(const QString &name) {
    return entries.value(name);
}
const FileEntry *
FileEntry::getEntry(const QString &name) const {
    return entries.value(name);
}

QHash<QString, FileEntryCache::Entry> FileEntryCache::entrycache;

FileEntryCache::FileEntryCache() {
}
FileEntryCache::~FileEntryCache() {
    QHash<QString, Entry>::const_iterator i = entrycache.constBegin();
    while (i != entrycache.constEnd()) {
        delete i.value().entry;
        ++i;
    }
}
void
FileEntryCache::addEntry(const QString& key, FileEntry*fe) {
    Entry& e = entrycache[key];
    e.lastUsed = QDateTime::currentDateTime();
    e.entry = fe;
    prune();
}
FileEntry*
FileEntryCache::getEntry(const QString& key, const QDateTime &mtime) {
    QHash<QString, Entry>::iterator e = entrycache.find(key);
    if (e == entrycache.end()) {
        return 0;
    }
    if (mtime != e.value().entry->mtime) {
        qDebug() << "file " << key << " changed " << mtime.secsTo(e.value().entry->mtime);
        delete e.value().entry;
        entrycache.erase(e);
        return 0;
    }
    e.value().lastUsed = QDateTime::currentDateTime();
    prune();
    return e.value().entry;
}
void
FileEntryCache::prune() {
    QHash<QString, Entry>::iterator i = entrycache.begin();
    QDateTime now = QDateTime::currentDateTime();
    while (i != entrycache.constEnd()) {
        if (i.value().lastUsed.secsTo(now) > 600) {
            delete i.value().entry;
            i = entrycache.erase(i);
        } else {
            ++i;
        }
    }
}

FileEntryCache ArchiveEngine::cache;
ArchiveEngine::ArchiveEngine(const QString &p, QFSFileEngine *fse)
    : streamengine(0), rootentry(0) {
    filestream = new FSFileInputStream(fse);
    parentstream = filestream;

    FileEntry* e = new FileEntry(0);
    e->size = fse->size();
    e->mtime = fse->fileTime(ModificationTime);
    e->atime = fse->fileTime(AccessTime);
    e->ctime = fse->fileTime(CreationTime);

    fullpath = p;
    int pos = p.lastIndexOf('/');
    if (pos != -1) {
        e->name = p.mid(pos+1);
        path = p.left(pos);
    } else {
        e->name = p;
    }
    entry = e;

    zipstream = 0;
    getRootEntry(e->mtime);
    openArchive();
}
ArchiveEngine::ArchiveEngine(StreamEngine *se)
        : streamengine(se), parentstream(se->getInputStream()), filestream(0),
            rootentry(0) {
    fullpath = se->fileName();
    entry = se->getFileEntry();
    zipstream = 0;
    getRootEntry(entry->mtime);
    openArchive();
}
ArchiveEngine::~ArchiveEngine() {
    if (zipstream) {
        delete zipstream;
    }
    foreach(InputStream *is, compressedstreams) {
        delete is;
    }
    if (filestream) {
        delete filestream;
        delete entry;
    }
    if (streamengine) {
        delete streamengine;
    }
    if (!readAllEntryNames) {
        delete rootentry;
    }
}
void
ArchiveEngine::getRootEntry(const QDateTime& mtime) {
    rootentry = cache.getEntry(fullpath, mtime);
    if (rootentry) {
        readAllEntryNames = true;
    } else {
        readAllEntryNames = false;
        rootentry = new FileEntry(0);
        rootentry->mtime = mtime;
    }
}
void
ArchiveEngine::openArchive() {
    if (zipstream) {
        delete zipstream;
    }
    if (parentstream == 0) {
//        printf("no stream\n");
        setError(QFile::FatalError, "no stream");
        return;
    }
    int32_t bufsize = 1000;
    parentstream->mark(bufsize); // make sure this is enough
    InputStream* compressed = parentstream;
    InputStream* decompressed = decompress(compressed, bufsize);
    while (decompressed) {
        compressed = decompressed;
        compressedstreams.append(compressed);
        compressed->mark(bufsize);
        decompressed = decompress(compressed, bufsize);
    }

    zipstream = new ZipInputStream(compressed);
    if (nextEntry()) {
        setError(QFile::NoError, "");
        return;
    }
    if (compressed->reset(0) != 0) {
        qDebug("ArchiveEngine mark call is too small.");
        return;
    }
    delete zipstream;
    zipstream = new TarInputStream(compressed);
    if (nextEntry()) {
        setError(QFile::NoError, "");
        return;
    }
    setError(QFile::FatalError, zipstream->getError());
    if (compressed->reset(0) != Ok) {
        qDebug("ArchiveEngine mark call is too small.");
        return;
    }
}
/*qint64
ArchiveEngine::read(char* data, qint64 maxlen) {
    if (maxlen == 0) {
        qDebug("maxlen == 0!!!\n");
        return true;
    }
    if (stream) {
        const char *start;
        int32_t nread;
        StreamStatus status;
        status = stream->read(start, nread, maxlen);
        if (status == InputStream::Ok) {
            memcpy(data, start, nread);
            return nread;
        }
        if (status == InputStream::Eof) {
            return 0;
        }
    }
    return -1;
}*/
bool
ArchiveEngine::testStream(InputStream* is, int32_t readsize) const {
    const char *start;
    is->mark(readsize);
    return is->read(start, readsize, readsize) == readsize;
}
InputStream*
ArchiveEngine::decompress(InputStream* is, int32_t bufsize) const {
    // try bzip
    InputStream *dec = new BZ2InputStream(is);
    if (testStream(dec, bufsize)) {
        if (dec->reset(0) != 0) {
            qDebug("ArchiveEngine mark call is too small.");
        }
        return dec;
    }
    delete dec;
    if (is->reset(0) != 0) {
        qDebug("ArchiveEngine mark call is too small.");
    }

    // try gzip
    dec = new GZipInputStream(is);
    if (testStream(dec, bufsize)) {
        dec->reset(0);
        return dec;
    }
    delete dec;
    is->reset(0);
    return 0;
}
void
ArchiveEngine::readEntryNames() const {
    while (nextEntry()) {}
    readAllEntryNames = true;
    cache.addEntry(fullpath, rootentry);
}
QStringList
ArchiveEngine::entryList(QDir::Filters /*filters*/,
        const QStringList& /*filterNames*/) const {
    // TODO: respect filters
    if (!readAllEntryNames) {
        readEntryNames();
    }
    QStringList e;
    foreach (FileEntry *fe, rootentry->getEntries()) {
        e.append(fe->name);
    }
    return e;
}
bool
ArchiveEngine::nextEntry() const {
    entrystream = zipstream->nextEntry();
    if (entrystream) {
        entrystream->mark(1);
        const EntryInfo& info = zipstream->getEntryInfo();
        QString name(info.filename.c_str());
        QStringList path = name.split("/", QString::SkipEmptyParts);
        FileEntry* fe = rootentry;
        foreach(QString s, path) {
            current = fe->getEntry(s);
            if (current == 0) {
                if (readAllEntryNames) {
                    qDebug("Archive was modified between accesses.");
                }
                current = fe->add(s);
            }
            fe = current;
        }
        current->fileFlags = 0;
        if (info.type == EntryInfo::File) {
            current->fileFlags |= QAbstractFileEngine::FileType;
        } else  if (info.type == EntryInfo::Dir) {
            current->fileFlags |= QAbstractFileEngine::DirectoryType;
        }
        current->size = info.size;
        current->mtime.setTime_t(info.mtime);
    }
    return entrystream;
}
StreamEngine*
ArchiveEngine::openEntry(const QString &filename) {
    if (entrystream == 0) return 0;
    // at least on entry must have been opened already
    do {
        //printf("openEntry %s %s\n", (const char*)current->name.toUtf8(), (const char*)filename.toUtf8());
        if (filename == current->name) {
            if (current->fileFlags & QAbstractFileEngine::FileType) {
                return new StreamEngine(current, this);
            }
            return 0;
        }
    } while (nextEntry());
    return 0;
}
ArchiveDirEngine*
ArchiveEngine::openDir(QString dirname) {
    if (entrystream == 0) return 0;
    do {
        if (dirname == current->name) {
            if (current->fileFlags & QAbstractFileEngine::DirectoryType) {
                return new ArchiveDirEngine(this, current);
            }
            return 0;
        }
    } while (nextEntry());
    return 0;
}
QString
ArchiveEngine::fileName(FileName file) const {
    if (streamengine) {
        return streamengine->fileName(file);
    }
    switch(file) {
    case PathName:
        return path;
    case BaseName:
        return entry->name;
    case DefaultName:
    default:
        if (path.isEmpty()) return entry->name;
        return fullpath;
    }
}
InputStream *
ArchiveEngine::getInputStream(const FileEntry* entry) {
    return (current == entry) ?entrystream :0;
}

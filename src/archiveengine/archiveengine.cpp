#include "archiveengine.h"
#include "archivedirengine.h"
#include "zipinputstream.h"
#include "tarinputstream.h"
#include "fsfileinputstream.h"
#include "streamengine.h"
#include "subinputstream.h"
#include "bz2inputstream.h"
#include "gzipinputstream.h"

FileEntry::~FileEntry() {
    foreach(FileEntry *fe, entries) {
        delete fe;
    }
}
FileEntry*
FileEntry::add(const QString &name) {
//    printf("adding '%s' to '%s'\n", (const char*)name.toUtf8(), (const char*)this->name.toUtf8());
    FileEntry *fe = new FileEntry(name);
    entries.append(fe);
    return fe;
}
FileEntry *
FileEntry::getEntry(const QString &name) {
    foreach(FileEntry *fe, entries) {
        if (fe->name == name) {
            return fe;
        }
    }
    return 0;
}
const FileEntry *
FileEntry::getEntry(const QString &name) const {
    foreach(FileEntry *fe, entries) {
        if (fe->name == name) {
            return fe;
        }
    }
    return 0;
}

ArchiveEngine::ArchiveEngine(const QString &p, FSFileInputStream *fs)
    : streamengine(0), parentstream(fs), filestream(fs), entry(0) {
    int pos = p.lastIndexOf('/');
    if (pos != -1) {
        entry.name = p.mid(pos+1);
        path = p.left(pos);
    } else {
        entry.name = p;
    }
    zipstream = 0;
    reopen();
}
ArchiveEngine::ArchiveEngine(StreamEngine *se)
        : streamengine(se), parentstream(se->getInputStream()), filestream(0), entry(0) {
    zipstream = 0;
    reopen();
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
    }
    if (streamengine) {
        delete streamengine;
    }
}
void
ArchiveEngine::reopen() {
    if (zipstream) {
        delete zipstream;
    }
    if (parentstream == 0) {
//        printf("no stream\n");
        setError(QFile::FatalError, "no stream");
        return;
    }
    size_t bufsize = 100;
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
    if (compressed->reset() != InputStream::Ok) {
        printf("ArchiveEngine mark call is too small.\n");
        return;
    }
    delete zipstream;
    zipstream = new TarInputStream(compressed);
    if (nextEntry()) {
        setError(QFile::NoError, "");
        return;
    }
    setError(QFile::FatalError, zipstream->getError().c_str());
    if (compressed->reset() != InputStream::Ok) {
        printf("ArchiveEngine mark call is too small.\n");
        return;
    }
}
bool
ArchiveEngine::testStream(InputStream* is, size_t readsize) const {
    const char *start;
    size_t nread;
    is->mark(readsize);
    return is->read(start, nread, readsize) == InputStream::Ok;
}
InputStream*
ArchiveEngine::decompress(InputStream* is, size_t bufsize) const {
    // try bzip
    InputStream *dec = new BZ2InputStream(is);
    if (testStream(dec, bufsize)) {
        dec->reset();
        return dec;
    }
    delete dec;
    is->reset();

    // try gzip
    dec = new GZipInputStream(is);
    if (testStream(dec, bufsize)) {
        dec->reset();
        return dec;
    }
    delete dec;
    is->reset();
    return 0;
}
void
ArchiveEngine::readEntryNames() const {
    while (nextEntry()) {}
}
QStringList
ArchiveEngine::entryList(QDir::Filters /*filters*/,
        const QStringList& /*filterNames*/) const {
    // TODO: respect filters
    readEntryNames();
    QStringList e;
    foreach (FileEntry *fe, entry.getEntries()) {
        e.append(fe->name);
    }
    return e;
}
bool
ArchiveEngine::nextEntry() const {
    entrystream = zipstream->nextEntry();
    if (entrystream) {
        const EntryInfo info = zipstream->getEntryInfo();
        QString name(info.filename.c_str());
        QStringList path = name.split("/", QString::SkipEmptyParts);
        FileEntry* fe = &entry;
        foreach(QString s, path) {
            current = fe->getEntry(s);
            if (current == 0) {
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
        current->size = entrystream->getSize();
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
        return entry.name;
    case DefaultName:
    default:
        if (path.isEmpty()) return entry.name;
        return path+"/"+entry.name;
    }
    return path;
}
InputStream *
ArchiveEngine::getInputStream(const FileEntry* entry) {
    return (current == entry) ?entrystream :0;
}

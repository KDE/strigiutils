#include "archiveenginehandler.h"
#include "fsfileinputstream.h"
#include "archivereader.h"
#include <QtCore/QAbstractFileEngine>
#include <QtCore/QFSFileEngine>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
using namespace jstreams;
using namespace std;
#include <dirent.h>

class QFileStreamOpener : public StreamOpener {
public:
    ~QFileStreamOpener() {}
    StreamBase<char>* openStream(const string& url);
    int stat(const string& url, EntryInfo& e);
};
StreamBase<char>*
QFileStreamOpener::openStream(const string& url) {
    StreamBase<char>* stream = new FSFileInputStream(QString(url.c_str()));
    if (stream->getStatus() != Ok) {
        delete stream;
        stream = 0;
    }
    return stream;
}
int
QFileStreamOpener::stat(const string& url, EntryInfo& e) {
    QFSFileEngine f(url.c_str());
    QAbstractFileEngine::FileFlags flags = f.fileFlags(
        QAbstractFileEngine::FileType|QAbstractFileEngine::DirectoryType);
    e.type = EntryInfo::Unknown;
    if (flags & QAbstractFileEngine::FileType) {
        e.type = EntryInfo::File;
    }
    if (flags & QAbstractFileEngine::DirectoryType) {
        e.type = EntryInfo::Dir;
    }
    e.size = f.size();
    e.mtime = f.fileTime(QAbstractFileEngine::ModificationTime).toTime_t();
    QByteArray filename = f.fileName(QAbstractFileEngine::BaseName).toUtf8();
    e.filename.assign((const char*)filename, filename.length());
//    printf("name:'%s'\n", e.filename.c_str());

    return 0;
}

class StreamFileEngine : public QAbstractFileEngine {
private:
    const string url;
    ArchiveReader* reader;
    StreamBase<char>* stream;
    EntryInfo entryinfo;
    
public:
    StreamFileEngine(ArchiveReader* r, const string& u) :url(u) {
        reader = r;
        stream = 0;
        reader->stat(u, entryinfo);
    }
    ~StreamFileEngine();
    bool isSequential () const {
        return true;
    }
    bool open(QIODevice::OpenMode /*mode*/) {
        return false;
    }
    bool close () { return true; }
    bool isRelativePath () const {
        return false;
    }
    bool caseSensitive () const { return true; }
    qint64 size() const { return entryinfo.size; }
    QDateTime fileTime ( FileTime time ) const {
        QDateTime d;
        d.setTime_t(entryinfo.mtime);
        return d;
    }
    QStringList entryList(QDir::Filters filters,
        const QStringList & filterNames) const {
        QStringList l;
        return l;
    }
    QString fileName ( FileName file) const {
//        printf("getname %s\n", url.c_str());
        switch(file) {
        case PathName:
            return url.c_str();
        case BaseName:
            return entryinfo.filename.c_str();
        case DefaultName:
        default:
            return url.c_str();
    }
        return QString(entryinfo.filename.c_str());
    }
};
StreamFileEngine::~StreamFileEngine() {
}
ArchiveEngineHandler::ArchiveEngineHandler() {
    reader = new ArchiveReader();
    opener = new QFileStreamOpener();
    reader->addStreamOpener(opener);
}
ArchiveEngineHandler::~ArchiveEngineHandler() {
    delete reader;
    delete opener;
}

QAbstractFileEngine *
ArchiveEngineHandler::create(const QString &fileName) const {
    static int i=0;
    printf("%i\n", i++);
//    return new StreamFileEngine(reader, (const char*)fileName.toUtf8());
//    return new QFSFileEngine(fileName);
    // try to open the deepest regular file in the file path
    QFSFileEngine* fse = new QFSFileEngine(fileName);
    QAbstractFileEngine::FileFlags flags = fse->fileFlags(
            QAbstractFileEngine::DirectoryType
            | QAbstractFileEngine::FileType
            | QAbstractFileEngine::ExistsFlag);
    if (flags & QAbstractFileEngine::DirectoryType) {
        return fse;
    }
    if (!(flags & QAbstractFileEngine::ExistsFlag)) {
        delete fse;
        fse = 0;
    } else if (!(flags & QAbstractFileEngine::FileType)) {
        return fse;
    }
    
    // use the full path
    QString path = fileName;
    while (fse == 0 && !path.isEmpty()) {
        fse = new QFSFileEngine(path);
        if (!fse->open(QIODevice::ReadOnly)) {
            delete fse;
            fse = 0;

            // strip off the last part of the path
            int pos = path.lastIndexOf('/');
            if (pos == -1) {
                path = "";
            } else {
                path = path.left(pos);
            }
        }
    } while (fse == 0 && !path.isEmpty());
    if (fse == 0) {
        // no file could be opened
        return 0;
    }
    delete fse;
    return new StreamFileEngine(reader, (const char*)fileName.toUtf8());
}

#include "streamengine.h"

StreamEngine::StreamEngine(const FileEntry* e, ArchiveEngineBase* engine)
    : entry(e), archive(engine) {
    stream = 0;
}
StreamEngine::~StreamEngine() {
    delete archive;
}
QString
StreamEngine::fileName(FileName file) const {
    switch(file) {
    case PathName:
        return archive->fileName(DefaultName);
    case BaseName:
        return entry->name;
    case DefaultName:
    default:
        return archive->fileName(DefaultName)+'/'+entry->name;
    }
}
InputStream *
StreamEngine::getInputStream() {
    if (stream == 0) {
        stream = archive->getInputStream(entry);
    }
    return stream;
}
bool
StreamEngine::open(QIODevice::OpenMode mode) {
    if (mode != QIODevice::ReadOnly) {
        return 0;
    }
    if (stream == 0) {
        stream = archive->getInputStream(entry);
    }
    return stream;
}
qint64
StreamEngine::read(char* data, qint64 maxlen) {
    if (maxlen == 0) {
        qDebug("maxlen == 0!!!\n");
        return true;
    }
    if (stream) {
        const char *start;
        size_t nread;
        InputStream::Status status;
        status = stream->read(start, nread, maxlen);
        if (status == InputStream::Ok) {
            bcopy(start, data, nread);
            return nread;
        }
        if (status == InputStream::Eof) {
            return 0;
        }
    }
    return -1;
}
/*qint64
StreamEngine::readLine(char *data, qint64 maxlen) {
    printf("readLine\n");
}*/
qint64
StreamEngine::size () const {
    return entry->size;
}

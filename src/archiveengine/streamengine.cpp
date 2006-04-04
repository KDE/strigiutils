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
    printf("open!\n");
/*    if (stream) {
        stream->mark(101);
        const char *start; size_t nread;
        InputStream::Status status;
        status = stream->read(start, nread, 100);
        if (status == InputStream::Ok) {
            stream->reset();
            printf("open: %i '%.*s'\n", nread, nread, start);
        }
    } */
    return stream;
}
qint64
StreamEngine::read(char* data, qint64 maxlen) {
        printf("read!!!\n");
    if (maxlen == 0) {
        printf("maxlen == 0!!!\n");
        return true;
    }
    if (stream) {
        const char *start;
        size_t nread;
        InputStream::Status status;
        status = stream->read(start, nread, maxlen);
        if (status == InputStream::Ok) {
            printf("read! %i %lli\n", nread, maxlen);
            bcopy(start, data, nread);
            return nread;
        }
        if (status == InputStream::Eof) {
            printf("eof! %i\n", nread);
            return 0;
        }
    }
    printf("read errorf!\n");
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

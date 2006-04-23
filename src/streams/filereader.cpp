#include "filereader.h"
using namespace jstreams;

FileReader::FileReader(const char* fname, const char* /*encoding_scheme*/,
        int32_t cachelen, int32_t /*cachebuff*/) {
    input = new FileInputStream(fname, cachelen);
    reader = new InputStreamReader(input);
}
FileReader::~FileReader() {
    if (reader) delete reader;
    if (input) delete input;
}
int32_t
FileReader::read(const wchar_t*& start) {
    int32_t nread = reader->read(start);
    if (nread == -1) {
        error = reader->getError();
        status = Error;
        return -1;
    } else if (nread == 0) {
        status = Eof;
    }
    return nread;
}
int32_t
FileReader::read(const wchar_t*& start, int32_t ntoread) {
    int32_t nread = reader->read(start, ntoread);
    if (nread == -1) {
        error = reader->getError();
        status = Error;
        return -1;
    } else if (nread != ntoread) {
        status = Eof;
    }
    return nread;
}
StreamStatus
FileReader::mark(int32_t readlimit) {
    status = reader->mark(readlimit);
    if (status != Ok) error = reader->getError();
    return status;
}
StreamStatus
FileReader::reset() {
    status = reader->reset();
    if (status != Ok) error = reader->getError();
    return status;
}


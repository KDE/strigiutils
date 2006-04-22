#include "subinputstream.h"
using namespace jstreams;

SubInputStream::SubInputStream(StreamBase<char> *i, int64_t length)
        : input(i) {
    size = length;
}
int32_t
SubInputStream::read(const char*& start) {
    return read(start, 1024);
}
int32_t
SubInputStream::read(const char*& start, int32_t ntoread) {
    const int64_t left = size - position;
    if (left == 0) {
        return 0;
    }
    // restrict the amount of data that can be read
    if (ntoread > left) {
        ntoread = left;
    }
    int32_t nread = input->read(start, ntoread);
    if (nread < 0) {
        status = Error;
        error = input->getError();
    } else {
        position += nread;
        if (position == size) {
            status = Eof;
        }
    }
    return nread;
}
StreamStatus
SubInputStream::mark(int32_t readlimit) {
    markPos = position;
    return input->mark(readlimit);
}
StreamStatus
SubInputStream::reset() {
    StreamStatus s = input->reset();
    if (s == Ok) {
        position = markPos;
    }
    return s;
}
int64_t
SubInputStream::skip(int64_t ntoskip) {
    const int64_t left = size - position;
    if (left == 0) {
        return 0;
    }
    // restrict the amount of data that can be skipped
    if (ntoskip > left) {
        ntoskip = left;
    }
    int64_t skipped = input->skip(ntoskip);
    if (input->getStatus() == Error) {
        status = Error;
        error = input->getError();
    } else {
        position += skipped;
        if (position == size) {
            status = Eof;
        }
    }
    return skipped;
}

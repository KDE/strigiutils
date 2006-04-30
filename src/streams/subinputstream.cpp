#include "jstreamsconfig.h"
#include "subinputstream.h"
using namespace jstreams;

SubInputStream::SubInputStream(StreamBase<char> *i, int64_t length)
        : offset(i->getPosition()), input(i) {
    size = length;
}
int32_t
SubInputStream::read(const char*& start, int32_t min, int32_t max) {
    const int64_t left = size - position;
    if (left == 0) {
        return 0;
    }
    // restrict the amount of data that can be read
    if (max <= 0 || max > left) {
        max = (int32_t)left;
    }
    int32_t nread = input->read(start, min, max);
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
int64_t
SubInputStream::mark(int32_t readlimit) {
    position = input->mark(readlimit) - offset;
    return position;
}
int64_t
SubInputStream::reset() {
    position = input->reset();
    if (position < 0) {
        status = Error;
        error = input->getError();
    } else {
        position -= offset;
    }
    return position;
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

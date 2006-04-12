#include "subinputstream.h"
using namespace jstreams;

SubInputStream::SubInputStream(InputStream *input, int32_t length)
        : size(length) {
    this->input = input;
    left = length;
}
StreamStatus
SubInputStream::read(const char*& start, int32_t& nread, int32_t max) {
    if (left == 0) {
        nread = 0;
        return Eof;
    }
    // restrict the amount of data that can be read
    if (max > left || max == 0) {
        max = (int32_t)((left > INT32MAX) ?INT32MAX :left);
    }
    status = input->read(start, nread, max);
    if (status) {
        //printf("suberror %s\n", input->getError().c_str());
        return status;
    }
    left -= nread;
    return Ok;
}
StreamStatus
SubInputStream::mark(int32_t readlimit) {
    markleft = left;
    return input->mark(readlimit);
}
StreamStatus
SubInputStream::reset() {
    left = markleft;
    return input->reset();
}
StreamStatus
SubInputStream::skipToEnd() {
    return skip(left);
}

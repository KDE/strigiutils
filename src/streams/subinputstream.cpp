#include "subinputstream.h"

SubInputStream::SubInputStream(InputStream *input, int32_t length)
        : size(length) {
    this->input = input;
    left = length;
}
InputStream::Status
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
InputStream::Status
SubInputStream::mark(int32_t readlimit) {
    markleft = left;
    return input->mark(readlimit);
}
InputStream::Status
SubInputStream::reset() {
    left = markleft;
    return input->reset();
}
InputStream::Status
SubInputStream::skipToEnd() {
    return skip(left);
}

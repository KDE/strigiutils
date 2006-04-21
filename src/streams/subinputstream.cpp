#include "subinputstream.h"
using namespace jstreams;

SubInputStream::SubInputStream(InputStream *input, int64_t length)
        : size(length) {
    this->input = input;
    left = length;
}
int32_t
SubInputStream::read(const char*& start) {
    int32_t ntoread = (int32_t)((left > INT32MAX) ?INT32MAX :left);
    int32_t nread = input->read(start, ntoread);
    if (ntoread != nread) {
        status = Error;
    }
    return nread;
}
int32_t
SubInputStream::read(const char*& start, int32_t ntoread) {
    if (left == 0) return 0;
    // restrict the amount of data that can be read
    if (ntoread > left) {
        ntoread = left;
    }
    int32_t nread = input->read(start, ntoread);
    if (ntoread != nread) {
        status = Error;
    }
    left -= nread;
    return nread;
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
int64_t
SubInputStream::skipToEnd() {
    return skip(left);
}

#include "Reader.h"

Reader::Status
Reader::read(wchar_t& c) {
    const wchar_t *buf;
    int32_t numRead;
    Status r;
    do {
        r = read(buf, numRead, 1);
    } while (r == Ok && numRead == 0);
    if (r == Ok) c = buf[0];
    return r;
}
Reader::Status
Reader::skip(int32_t ntoskip) {
    const wchar_t *begin;
    int32_t nread;
    while (ntoskip) {
        Status r = read(begin, nread, ntoskip);
        if (r != Ok) return r;
        ntoskip -= nread;
    }
    return Ok;
}

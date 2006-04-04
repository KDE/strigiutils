#include "inputstream.h"

InputStream::Status
InputStream::skip(size_t ntoskip) {
    const char *begin;
    size_t nread;
    while (ntoskip) {
        Status r = read(begin, nread, ntoskip);
        if (r != Ok) return r;
        ntoskip -= nread;
    }
    return Ok;
}

#include "inputstream.h"

InputStream::Status
InputStream::skip(int64_t ntoskip) {
    const char *begin;
    int32_t nread;
    while (ntoskip) {
        Status r = read(begin, nread, ntoskip);
        if (r != Ok) return r;
        ntoskip -= nread;
    }
    return Ok;
}

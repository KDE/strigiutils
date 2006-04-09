#include "inputstream.h"
#include <limits>

InputStream::Status
InputStream::skip(int64_t ntoskip, int64_t* skipped) {
    const char *begin;
    int32_t nread;
    if (skipped) *skipped = 0;
    while (ntoskip) {
        int32_t readstep = (int32_t)((ntoskip > INT32MAX) ?INT32MAX :ntoskip);
        Status r = read(begin, nread, readstep);
        if (r != Ok) return r;
        ntoskip -= nread;
        if (skipped) *skipped += nread;
    }
    return Ok;
}

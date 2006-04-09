#include "streamendanalyzer.h"
#include "inputstream.h"

char
StreamEndAnalyzer::testStream(InputStream *in) {
    int32_t testsize = 1;
    const char *dummyptr;
    int32_t dummy;
    char r = in->mark(testsize);
    if (r) {
        return r;
    }
    r = in->read(dummyptr, dummy, testsize);
    if (r) {
        return r;
    }
    r = in->reset();
    if (r) {
        return r;
    }
    return 0;
}

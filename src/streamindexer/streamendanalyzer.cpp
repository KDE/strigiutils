#include "streamendanalyzer.h"
#include "inputstream.h"

char
StreamEndAnalyzer::testStream(InputStream *in) {
    int32_t testsize = 1;
    const char *dummyptr;
    int32_t dummy;
    InputStream::Status r = in->mark(testsize);
    if (r != InputStream::Ok) {
        return -1;
    }
    r = in->read(dummyptr, dummy, testsize);
    if (r != InputStream::Ok) {
        return -1;
    }
    r = in->reset();
    if (r != InputStream::Ok) {
        return -1;
    }
    return 0;
}

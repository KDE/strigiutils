#include "streamendanalyzer.h"
#include "inputstream.h"
using namespace jstreams;

char
StreamEndAnalyzer::testStream(InputStream *in) {
    int32_t testsize = 1;
    const char *dummyptr;
    int32_t nread;
    StreamStatus r = in->mark(testsize);
    if (r != Ok) {
        return -1;
    }
    nread = in->read(dummyptr, testsize, testsize);
    if (nread != testsize) {
        return -1;
    }
    r = in->reset();
    if (r != Ok) {
        return -1;
    }
    return 0;
}

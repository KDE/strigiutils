#include "TarInputStreamTest.h"
#include "../fileinputstream.h"
#include "../tarinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
TarInputStreamTest::testStream() {
    for (int i=0; i<nstreamprovidertests; ++i) {
        FileInputStream file("a.tar");
        TarInputStream tar(&file);
        streamprovidertests[i](&tar);
    }
}


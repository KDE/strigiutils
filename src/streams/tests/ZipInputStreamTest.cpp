#include "ZipInputStreamTest.h"
#include "../fileinputstream.h"
#include "../zipinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
ZipInputStreamTest::testStream() {
    for (int i=0; i<nstreamprovidertests; ++i) {
        FileInputStream file("a.zip");
        ZipInputStream zip(&file);
        streamprovidertests[i](&zip);
    }
}


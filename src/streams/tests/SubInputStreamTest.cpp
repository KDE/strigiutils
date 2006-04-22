#include "SubInputStreamTest.h"
#include "../fileinputstream.h"
#include "../subinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
SubInputStreamTest::testStream() {
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.zip");
        SubInputStream sub(&file, 1);
        charinputstreamtests[i](&sub);
    }
}


#include "FileInputStreamTest.h"
#include "../fileinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
FileInputStreamTest::testStream() {
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.zip");
        charinputstreamtests[i](&file);
    }
}


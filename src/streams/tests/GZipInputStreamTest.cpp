#include "GZipInputStreamTest.h"
#include "../fileinputstream.h"
#include "../gzipinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
GZipInputStreamTest::testStream() {
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.gz");
        GZipInputStream gzip(&file);
        charinputstreamtests[i](&file);
    }
}


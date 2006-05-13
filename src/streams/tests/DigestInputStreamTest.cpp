#include "DigestInputStreamTest.h"
#include "../fileinputstream.h"
#include "../digestinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
DigestInputStreamTest::testStream() {
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.zip");
        DigestInputStream sub(&file);
        charinputstreamtests[i](&sub);
    }
}


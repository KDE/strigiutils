#include "ZipInputStreamTest.h"
#include "../fileinputstream.h"
#include "../zipinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
ZipInputStreamTest::testStream() {
    FileInputStream file("a.zip");
    ZipInputStream zip(&file);
    subStreamProviderTest1(&zip);
}


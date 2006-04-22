#include "SubInputStreamTest.h"
#include "../fileinputstream.h"
#include "../subinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
SubInputStreamTest::testStream() {
    FileInputStream file("a.zip");
    SubInputStream sub(&file, 1);
    inputStreamTest1<char>(&sub);
}


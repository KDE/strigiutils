#include "FileInputStreamTest"
#include "../fileinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
FileInputStreamTest::testStream() {
    FileInputStream file("a.zip");
    inputStreamTest1<char>(&file);
}


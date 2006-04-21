#include "GZipInputStreamTest"
#include "../fileinputstream.h"
#include "../gzipinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
GZipInputStreamTest::testStream() {
    FileInputStream file("a.gz");
    GZipInputStream gzip(&file);
    inputStreamTest1<char>(&gzip);
}


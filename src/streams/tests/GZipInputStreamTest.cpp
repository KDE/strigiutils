#include "GZipInputStreamTest"
#include "../fileinputstream.h"
#include "../gzipinputstream.h"
#include <QtTest/QtTest>
using namespace jstreams;

void
GZipInputStreamTest::testStream() {
    FileInputStream file("a.gz");
    GZipInputStream gzip(&file);
    const char* start;
    int32_t size = gzip.read(start);
    QVERIFY(size == 275);
}


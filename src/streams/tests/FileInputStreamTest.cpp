#include "FileInputStreamTest"
#include "../fileinputstream.h"
#include <QtTest/QtTest>
using namespace jstreams;

void
FileInputStreamTest::testStream() {
    FileInputStream file("a.zip");
    const char *ptr;
    int32_t size = file.read(ptr, 100000000);
    printf("size: %i\n", size);
    QVERIFY(size == 275);
}


#include "TarInputStreamTest"
#include "../fileinputstream.h"
#include "../tarinputstream.h"
#include <QtTest/QtTest>
using namespace jstreams;

void
TarInputStreamTest::testStream() {
    FileInputStream file("a.tar");
    TarInputStream tar(&file);
    SubInputStream *s = tar.nextEntry();
    int count = 0;
    const char* ptr;
    while (s) {
        int32_t size = tar.getEntryInfo().size;
        int32_t n = s->read(ptr, 1000000);
        QVERIFY(size == n);
        s = tar.nextEntry();
        count++;
    }
    QVERIFY(count == 2);
}


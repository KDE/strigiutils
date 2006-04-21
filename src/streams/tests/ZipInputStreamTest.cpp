#include "ZipInputStreamTest"
#include "../fileinputstream.h"
#include "../zipinputstream.h"
#include <QtTest/QtTest>
using namespace jstreams;

void
ZipInputStreamTest::testStream() {
    FileInputStream file("a.zip");
    ZipInputStream zip(&file);
    SubInputStream *s = zip.nextEntry();
    int count = 0;
    const char* ptr;
    while (s) {
        int32_t size = zip.getEntryInfo().size;
        int32_t n = s->read(ptr, size+10);
        QVERIFY(size == n);
        s = zip.nextEntry();
        count++;
    }
    if (zip.getStatus() == jstreams::Error) {
        printf("%s\n", zip.getError());
    }
    QVERIFY(zip.getStatus() == jstreams::Eof);
    QVERIFY(count == 2);
}


#include "ZipInputStreamTest"
#include "../fileinputstream.h"
#include "../zipinputstream.h"
#include <QtTest/QtTest>
using namespace jstreams;

void
ZipInputStreamTest::testStream() {
    FileInputStream file("a.zip");
    ZipInputStream zip(&file);
    SubInputStream *s;
    s = zip.nextEntry();
    QVERIFY(s);
}


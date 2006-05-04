#include "StringTerminatedSubStreamTest.h"
#include "../fileinputstream.h"
#include "../stringreader.h"
#include "../stringterminatedsubstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
StringTerminatedSubStreamTest::testStream() {
    StringReader<char> sr("abc");
    StringTerminatedSubStream sub(&sr, "b");
    const char* start;
    int64_t nread = sub.read(start, 10, 10);
    printf("read %lli\n", nread);
/*
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.zip");
        StringTerminatedSubStream sub(&file, "THEEND");
        charinputstreamtests[i](&sub);
    }*/
}


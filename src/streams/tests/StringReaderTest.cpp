#include "StringReaderTest.h"
#include "../stringreader.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
StringReaderTest::testStream() {
    for (int i=0; i<ninputstreamtests; ++i) {
        StringReader<char> s1("a.zip", 5);
        charinputstreamtests[i](&s1);
    }
}


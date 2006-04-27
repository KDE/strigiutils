#include "MailInputStreamTest.h"
#include "../fileinputstream.h"
#include "../mailinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
MailInputStreamTest::testStream() {
    for (int i=0; i<nstreamprovidertests; ++i) {
        FileInputStream file("a.zip");
        MailInputStream mail(&file);
        streamprovidertests[i](&mail);
    }
}


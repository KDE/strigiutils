#include "InputStreamReaderTest.h"
#include "../inputstreamreader.h"
#include "../fileinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
InputStreamReaderTest::testStream() {
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("home.de.html");
        InputStreamReader isr(&file);
        wcharinputstreamtests[i](&isr);
    }
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("home.cn.html");
        InputStreamReader isr(&file, "GB2312");
        wcharinputstreamtests[i](&isr);
    }
}

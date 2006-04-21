#include "TarInputStreamTest"
#include "../fileinputstream.h"
#include "../tarinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
TarInputStreamTest::testStream() {
    FileInputStream file("a.tar");
    TarInputStream tar(&file);
    subStreamProviderTest1(&tar);
}


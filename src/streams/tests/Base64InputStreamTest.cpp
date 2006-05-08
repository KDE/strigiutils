#include "Base64InputStreamTest.h"
#include "../base64inputstream.h"
#include "../fileinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
Base64InputStreamTest::testStream() {
    FileInputStream file("base64enc.txt");
    Base64InputStream b64(&file);
    b64.mark(1);
    const char* start;
    int32_t nread = b64.read(start, 1, 0);
    while (nread > 0) {
        //printf("%i\n", nread);
        for (int i=0; i<nread; ++i) {
            printf("%c", start[i]);
        }
        nread = b64.read(start, 1, 0);
    }
    printf("\n");
/*    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.zip");
        charinputstreamtests[i](&file);
    }*/
}


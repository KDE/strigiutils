#include "pdfparser.h"
#include "fileinputstream.h"
using namespace jstreams;
extern int32_t streamcount;

class SaveStreamHandler : public PdfParser::StreamHandler {
private:
    bool handle(jstreams::StreamBase<char>*) {
        return true;
    }
};

int
main(int argc, char** argv) {
    streamcount = 0;
    PdfParser parser;
    SaveStreamHandler saver;
    parser.setStreamHandler(&saver);
    for (int i=1; i<argc; ++i) {
        // check if we can read the file
        FILE* f = fopen(argv[i], "rb");
        if (f == 0) continue;
        fclose(f);

        // parse the file
        FileInputStream file(argv[i]);
        StreamStatus r = parser.parse(&file);
        if (r != Eof) {
            printf("error in %s\n", argv[i]);
        }
    }
    return 0;
}

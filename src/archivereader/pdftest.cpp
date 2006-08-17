#include "fileinputstream.h"
using namespace jstreams;

/*
 * parser for files like this one:
 * http://partners.adobe.com/public/developer/en/pdf/PDFReference16.pdf
 */

int
main(int argc, char** argv) {
    for (int i=1; i<argc; ++i) {
        FILE* file = fopen(argv[i], "rb");
        fseek(file, 0, SEEK_END);
        int32_t size = ftell(file);
        fseek(file, 0, SEEK_SET);
        fclose(file);
        void*p = realloc(0, 1);
        free(p);
//        FileInputStream file(argv[i]);
    }
    return 0;
}

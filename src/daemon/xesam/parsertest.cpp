#include "xesamuserparser.h"

int
main(int argc, char** argv) {
    XesamUserParser parser;
    for (int i=1; i<argc; ++i) {
        parser.parse(argv[i]);
    }
    return 0;
}

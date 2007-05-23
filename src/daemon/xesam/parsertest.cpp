#include "queryparser.h"

int
main(int argc, char** argv) {
    Strigi::QueryParser parser;
    for (int i=1; i<argc; ++i) {
        parser.buildQuery(argv[i]);
    }
    return 0;
}

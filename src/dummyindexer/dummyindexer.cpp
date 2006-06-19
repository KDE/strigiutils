#include "indexer.h"

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s [-v verbosity] [dir-to-index]\n", argv[0]);
}

int
main(int argc, char **argv) {
    if (argc != 2 && argc != 4) {
        printUsage(argv);
        return -1;
    }
    int verbosity = 0;
    if (argc == 4) {
        if (strcmp("-v", argv[1])) {
            printUsage(argv);
            return -1;
        }
        verbosity = atoi(argv[2]);
    }
         
    Indexer indexer(verbosity);
    indexer.index(argv[argc-1]);
    return 0;
}

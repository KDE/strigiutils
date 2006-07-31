#include "filelister.h"
#include <stdio.h>

/**
 * This test file can be used to measure the performance of the filelister
 * class. The speed can be compared to e.g. "find [path] -printf ''".
 **/

bool
addFileCallback(const char* path, uint dirlen, uint len, time_t mtime) {
    printf("%s\n", path);
}

int
main(int argc, char** argv) {
    if (argc != 2) {
        return -1;
    }
    FileLister lister;
    lister.setCallbackFunction(&addFileCallback);
    lister.listFiles(argv[1]);
    return 0;
}

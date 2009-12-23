/*
 * Some relatively high level tests for checking the indexer.
 * Writing of tests is often triggered by bugs encountered, which makes the collection look miscellaneous.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <unistd.h>

/*
 * Test if having directories that are not accessible influences the indexing.
 * Scenario is this similar to running these command-line commands:
 *   mkdir a b c
 *   echo hi > b/hi
 *   chmod a-r a c
 *   deepfind .
 * The deepfind run should find the file 'b/hi'.
 */
int
testScanWithUnreadableDir() {
    // setup
    if (mkdir("testScanWithUnreadableDir", 0700) != 0
            || mkdir("testScanWithUnreadableDir/a", 0700) != 0
            || mkdir("testScanWithUnreadableDir/b", 0700) != 0
            || mkdir("testScanWithUnreadableDir/c", 0700) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    FILE * file = fopen("testScanWithUnreadableDir/b/hello", "w");
    if (file == NULL || fwrite("world", 1, 5, file) != 5 || fclose(file) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }

    // test

    // teardown
    if (unlink("testScanWithUnreadableDir/b/hello") != 0
            || rmdir("testScanWithUnreadableDir/a") != 0
            || rmdir("testScanWithUnreadableDir/b") != 0
            || rmdir("testScanWithUnreadableDir/c") != 0
            || rmdir("testScanWithUnreadableDir") != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/*
 * Main function calling all the individual tests.
 */
int
indextests(int argc, char* argv[]) {
    if (testScanWithUnreadableDir() != 0) return -1;
    return 0;
}

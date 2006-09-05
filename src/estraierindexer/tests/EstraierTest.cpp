#include "jstreamsconfig.h"
#include "estraierindexmanager.h"
#include "indexwritertests.h"
#include "indexreadertests.h"
#include <sys/stat.h>
#include <sys/types.h>

int
EstraierTest(int argc, char**argv) {
    const char* path = "testestraierindex";
    int errors = 0;

    // initialize a directory for writing and an indexmanager
    mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR);
    EstraierIndexManager* manager = new EstraierIndexManager(path);

    jstreams::IndexWriter* writer = manager->getIndexWriter();
    IndexWriterTests tests(writer);
    errors += tests.testAll();

    jstreams::IndexReader* reader = manager->getIndexReader();
    IndexReaderTests rtests(reader);
    errors += rtests.testAll();

    // close and clean up the manager
    delete manager;

    // clean up data
    std::string cmd = "rm -r ";
    cmd += path;
    system(cmd.c_str());

    return errors;
}

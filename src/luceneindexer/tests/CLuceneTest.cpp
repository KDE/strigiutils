#include "jstreamsconfig.h"
#include "cluceneindexmanager.h"
//#include "indexmanagertests.h"
#include "indexwritertests.h"
#include "indexreadertests.h"
#include <sys/stat.h>
#include <sys/types.h>

int
CLuceneTest(int argc, char**argv) {
    const char* path = "testcluceneindex";
    int errors = 0;

    // initialize a directory for writing and an indexmanager
    mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR);
    jstreams::IndexManager* manager = createCLuceneIndexManager(path);

//    IndexManagerTests tests(manager);
//    errors += tests.testAllInThreads(20);

    jstreams::IndexWriter* writer = manager->getIndexWriter();
    IndexWriterTests wtests(writer);
    errors += wtests.testAll();

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

#include "jstreamsconfig.h"
#include "sqliteindexmanager.h"
#include "indexmanagertests.h"
#include "indexwritertests.h"
#include "indexreadertests.h"
#include "indexerconfiguration.h"
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

StrigiMutex errorlock;
int founderrors = 0;
int
SqliteTest(int argc, char**argv) {
    const char* path = "testsqliteindex";

    // initialize a directory for writing and an indexmanager
    mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR);
    string p(path);
    p += "/test.db";
    jstreams::IndexManager* manager = createSqliteIndexManager(p.c_str());

    jstreams::IndexerConfiguration ic;
    IndexManagerTests tests(manager, ic);
    tests.testAll();
/*    tests.testAllInThreads(20);

    jstreams::IndexWriter* writer = manager->getIndexWriter();
    IndexWriterTests wtests(*writer, ic);
    wtests.testAll();

    jstreams::IndexReader* reader = manager->getIndexReader();
    IndexReaderTests rtests(reader);
    rtests.testAll();
*/
    // close and clean up the manager
    delete manager;

    // clean up data
    std::string cmd = "rm -r ";
    cmd += path;
    system(cmd.c_str());
    fprintf(stderr, "bye\n");

    return founderrors;
}

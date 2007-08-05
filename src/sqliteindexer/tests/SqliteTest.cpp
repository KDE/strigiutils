#include <strigi/strigiconfig.h>
#include "sqliteindexmanager.h"
#include "indexmanagertests.h"
#include "indexwritertests.h"
#include "indexreadertests.h"
#include "analyzerconfiguration.h"
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
    Strigi::IndexManager* manager = createSqliteIndexManager(p.c_str());

    Strigi::AnalyzerConfiguration ic;
    IndexManagerTests tests(manager, ic);
    tests.testAll();
/*    tests.testAllInThreads(20);

    Strigi::IndexWriter* writer = manager->indexWriter();
    IndexWriterTests wtests(*writer, ic);
    wtests.testAll();

    Strigi::IndexReader* reader = manager->indexReader();
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

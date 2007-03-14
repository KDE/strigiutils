#include "jstreamsconfig.h"
#include "estraierindexmanager.h"
#include "indexmanagertests.h"
#include "indexwritertests.h"
#include "indexreadertests.h"
#include "analyzerconfiguration.h"
#include <sys/stat.h>
#include <sys/types.h>

StrigiMutex errorlock;
int founderrors = 0;
int
EstraierTest(int argc, char**argv) {
    const char* path = "testestraierindex";

    // initialize a directory for writing and an indexmanager
    mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR);
    EstraierIndexManager* manager = new EstraierIndexManager(path);

    Strigi::AnalyzerConfiguration ic;
    IndexManagerTests tests(manager, ic);
    tests.testAll();
    tests.testAllInThreads(20);

    Strigi::IndexWriter* writer = manager->getIndexWriter();
    IndexWriterTests wtests(*writer, ic);
    wtests.testAll();

    Strigi::IndexReader* reader = manager->getIndexReader();
    IndexReaderTests rtests(reader);
    rtests.testAll();

    // close and clean up the manager
    delete manager;

    // clean up data
    std::string cmd = "rm -r ";
    cmd += path;
//    system(cmd.c_str());
    return founderrors;
}

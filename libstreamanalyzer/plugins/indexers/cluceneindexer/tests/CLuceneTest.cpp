#include <strigi/strigiconfig.h>
#include "compat.h"
#include "indexpluginloader.h"
#include "indexmanager.h"
#include "indexmanagertests.h"
#include "indexwritertests.h"
#include "indexreadertests.h"
#include <strigi/analyzerconfiguration.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;

StrigiMutex errorlock;
int founderrors = 0;
int
CLuceneTest(int argc, char*argv[]) {
    setenv("STRIGI_PLUGIN_PATH", BINARYDIR"/src/luceneindexer/", 1);
    setenv("XDG_DATA_HOME",
        SOURCEDIR"/src/streamanalyzer/fieldproperties", 1);
    setenv("XDG_DATA_DIRS",
        SOURCEDIR"/src/streamanalyzer/fieldproperties", 1);
    const char* path = ":memory:";

    // initialize a directory for writing and an indexmanager
    Strigi::IndexManager* manager
        = Strigi::IndexPluginLoader::createIndexManager("clucene", path);
    if (manager == 0) {
        cerr << "could not create indexmanager" << endl;
        founderrors++;
        return founderrors;
    }

    Strigi::AnalyzerConfiguration ic;
    IndexManagerTests tests(manager, ic);
    tests.testAll();
    tests.testAllInThreads(20);

    Strigi::IndexWriter* writer = manager->indexWriter();
    IndexWriterTests wtests(*writer, ic);
    wtests.testAll();

    Strigi::IndexReader* reader = manager->indexReader();
    IndexReaderTests rtests(reader);
    rtests.testAll();

    // close and clean up the manager
    Strigi::IndexPluginLoader::deleteIndexManager(manager);

    return founderrors;
}

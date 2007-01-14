#include "jstreamsconfig.h"
#include "sqliteindexmanager.h"
#include "indexwriter.h"
#include "indexreader.h"
#include "indexerconfiguration.h"
#include "indexable.h"
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;
using namespace jstreams;

int
addAndCount(IndexWriter* writer, IndexReader* reader, int m) {
    if (writer == 0) return 1;
    writer->deleteAllEntries();
    IndexerConfiguration ic;
    StreamIndexer si(ic);
    ostringstream str;
    for (int i=0; i<m; ++i) {
        str << "/" << i;
        string s(str.str());
        { Indexable idx(s, 0, *writer, si); }
        str.str("");
    }
    writer->commit();
    return 0;
/*    int n = reader->countDocuments();
    if (n != m) fprintf(stderr, "%i != %i\n", n, m);
    return m - n;*/
}

int
simpletest(int argc, char**argv) {
    const char* path = "testsqliteindex";

    // initialize a directory for writing and an indexmanager
    mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR);
    string p(path);
    p += "/test.db";
    IndexManager* manager = createSqliteIndexManager(p.c_str());
    IndexWriter* writer = manager->getIndexWriter();
    IndexReader* reader = manager->getIndexReader();
    addAndCount(writer, reader, 1);

    // close and clean up the manager
    delete manager;

    // clean up data
/*    std::string cmd = "rm -r ";
    cmd += path;
    system(cmd.c_str());
    fprintf(stderr, "bye\n");
*/
    return 1;
}

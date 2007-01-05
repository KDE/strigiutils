#include "jstreamsconfig.h"
#include "indexmanagertests.h"
#include "indexreader.h"
#include "indexwriter.h"
#include "indexable.h"
#include "indexmanager.h"
#include "indexerconfiguration.h"
#include "query.h"
#include "strigi_thread.h"
#include <sstream>
#include <iostream>
using namespace std;
using namespace jstreams;

class IndexManagerTester {
private:
    StrigiMutex lock;
    IndexManager* manager;
    IndexWriter* writer;
    IndexReader* reader;
    StreamIndexer si;
public:
    IndexManagerTester(IndexManager* m, IndexerConfiguration& ic)
            : manager(m), writer(manager->getIndexWriter()),
              si(*writer, ic) {
        reader = manager->getIndexReader();
    }
    ~IndexManagerTester() {
    }
    void runUnthreadedTests();
    void runThreadedTests();
    void addAndCount(int m = 20);
    void testNumberQuery();
};
void
IndexManagerTester::runUnthreadedTests() {

    // tests that only need return 0 when not threaded
    addAndCount();
    testNumberQuery();

}
void
IndexManagerTester::runThreadedTests() {
    // tests that only need return 0 when not threaded
    addAndCount();
    testNumberQuery();
}
void
IndexManagerTester::addAndCount(int m) {
    VERIFY(writer);
    if (writer == 0) return;
    writer->deleteAllEntries();
    ostringstream str;
    for (int i=0; i<m; ++i) {
        str << "/" << i;
        string s(str.str());
        { Indexable idx(s, 0, *writer, si); }
        str.str("");
    }
    writer->commit();
    int n = reader->countDocuments();
    if (n != m) fprintf(stderr, "%i != %i\n", n, m);
    VERIFY(n == m);
}
void
IndexManagerTester::testNumberQuery() {
    VERIFY(writer);
    if (writer == 0) return;
    writer->deleteAllEntries();
    // add numbers to the database
    int m = 200;
    ostringstream str;
    string size("size");
    for (int i=1; i<=m; ++i) {
        str << i;
        string value(str.str());
        string name('/'+value);
        {
             Indexable idx(name, 0, *writer, si);
             idx.setField(size, value);
        }
        str.str("");
    }
    writer->commit();
    QueryParser parser;
    Query q = parser.buildQuery("size:>0", -1, 0);
    int count = reader->countHits(q);
    if (count != m) fprintf(stderr, "%i != %i\n", count, m);
    VERIFY(count == m);
}
/* below here the threading plumbing is done */
STRIGI_THREAD_FUNCTION(threadstarter,d) {
//    IndexManagerTests* tester = static_cast<IndexManagerTests*>(d);
//    tester->runThreadedTests();
    STRIGI_THREAD_EXIT(0);
    return 0;
}
IndexManagerTests::IndexManagerTests(jstreams::IndexManager* m,
        IndexerConfiguration& ic)
    :tester (new IndexManagerTester(m, ic)) {
}
IndexManagerTests::~IndexManagerTests() {
    delete tester;
}
void
IndexManagerTests::testAllInThreads(int n) {
    STRIGI_THREAD_TYPE* thread = new STRIGI_THREAD_TYPE[n];
    for (int i=0; i<n; ++i) {
        STRIGI_THREAD_CREATE(&thread[i], threadstarter, this);
    }
    for (int i=0; i<n; ++i) {
        STRIGI_THREAD_JOIN(thread[i]);
    }

    delete [] thread;
}

void
IndexManagerTests::testAll() {
    tester->runUnthreadedTests();
    tester->runThreadedTests();
}
void
IndexManagerTests::runUnthreadedTests() {
    tester->runUnthreadedTests();
}
void
IndexManagerTests::runThreadedTests() {
    tester->runThreadedTests();
}

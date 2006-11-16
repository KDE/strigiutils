#include "jstreamsconfig.h"
#include "indexmanagertests.h"
#include "indexreader.h"
#include "indexable.h"
#include "indexmanager.h"
#include "query.h"
#include "strigi_thread.h"
#include <sstream>
#include <iostream>
using namespace std;
using namespace jstreams;

class IndexManagerTester {
private:
    STRIGI_MUTEX_DEFINE(lock);
    int errors;
    IndexManager* manager;
    IndexWriter* writer;
    IndexReader* reader;
    StreamIndexer si;
public:
    IndexManagerTester(IndexManager* m) :errors(0), manager(m),
            writer(manager->getIndexWriter()), si(writer) {
        reader = manager->getIndexReader();
        STRIGI_MUTEX_INIT(&lock);
    }
    ~IndexManagerTester() {
        STRIGI_MUTEX_DESTROY(&lock);
    }
    int runUnthreadedTests();
    int runThreadedTests();
    void cleanErrors() {
        STRIGI_MUTEX_LOCK(&lock);
        errors = 0;
        STRIGI_MUTEX_UNLOCK(&lock);
    }
    int getErrors() {
        int n;
        STRIGI_MUTEX_LOCK(&lock);
        n = errors;
        STRIGI_MUTEX_UNLOCK(&lock);
        return errors;
    }
    void addErrors(int n) {
        STRIGI_MUTEX_LOCK(&lock);
        errors += n;
        STRIGI_MUTEX_UNLOCK(&lock);
    }
    int addAndCount();
    int testNumberQuery();
};
int
IndexManagerTester::runUnthreadedTests() {
    int n = 0;

    // tests that only need return 0 when not threaded
    n += addAndCount();
    n += testNumberQuery();

    addErrors(n);
    return n;
}
int
IndexManagerTester::runThreadedTests() {
    int n = 0;
    // tests that only need return 0 when not threaded
    addAndCount();
    testNumberQuery();
    addErrors(n);
    return n;
}
int
IndexManagerTester::addAndCount() {
    writer->deleteAllEntries();
    int m = 20;
    ostringstream str;
    for (int i=0; i<m; ++i) {
        str << "/" << i;
        string s(str.str());
        { Indexable idx(s, 0, *writer, si); }
        str.str("");
    }
    writer->commit();
    int n = reader->countDocuments();
    return n != m;
}
int
IndexManagerTester::testNumberQuery() {
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
    Query q("size:>0", -1, 0);
    int count = reader->countHits(q);
    printf("count: %i\n", count);
    return count != m;
}
/* below here the threading plumbing is done */
void*
threadstarter(void *d) {
    IndexManagerTests* tester = static_cast<IndexManagerTests*>(d);
//    tester->runThreadedTests();
    STRIGI_THREAD_EXIT(0);
}
IndexManagerTests::IndexManagerTests(jstreams::IndexManager* m)
    :tester (new IndexManagerTester(m)) {
}
IndexManagerTests::~IndexManagerTests() {
    delete tester;
}
int
IndexManagerTests::testAllInThreads(int n) {
    tester->cleanErrors();
    STRIGI_THREAD_TYPE* thread = new STRIGI_THREAD_TYPE[n];
    for (int i=0; i<n; ++i) {
        STRIGI_THREAD_CREATE(thread+i, threadstarter, this);
    }
    for (int i=0; i<n; ++i) {
        STRIGI_THREAD_JOIN(thread[i], 0);
    }

    delete [] thread;
    return tester->getErrors();
}

int
IndexManagerTests::testAll() {
    tester->cleanErrors();
    int n = 0;
    n += tester->runUnthreadedTests();
    n += tester->runThreadedTests();
    return n;
}
int
IndexManagerTests::runUnthreadedTests() {
    return tester->runUnthreadedTests();
}
int
IndexManagerTests::runThreadedTests() {
    return tester->runThreadedTests();
}

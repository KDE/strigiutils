#include "indexmanagertests.h"
#include "indexreader.h"
#include "indexwriter.h"
#include "indexmanager.h"
#include "query.h"
#include <sstream>
#include <iostream>
using namespace std;
using namespace jstreams;

class IndexManagerTester {
private:
    pthread_mutex_t lock;
    int errors;
    IndexManager* manager;
    IndexWriter* writer;
    IndexReader* reader;
public:
    IndexManagerTester(IndexManager* m) :errors(0), manager(m) {
        writer = manager->getIndexWriter();
        reader = manager->getIndexReader();
        pthread_mutex_init(&lock, 0);
    }
    ~IndexManagerTester() {
        pthread_mutex_destroy(&lock);
    }
    int runUnthreadedTests();
    int runThreadedTests();
    void cleanErrors() {
        pthread_mutex_lock(&lock);
        errors = 0;
        pthread_mutex_unlock(&lock);
    }
    int getErrors() {
        int n;
        pthread_mutex_lock(&lock);
        n = errors;
        pthread_mutex_unlock(&lock);
        return errors;
    }
    void addErrors(int n) {
        pthread_mutex_lock(&lock);
        errors += n;
        pthread_mutex_unlock(&lock);
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
    addErrors(n);
    return n;
}
int
IndexManagerTester::addAndCount() {
    writer->deleteAllEntries();
    int m = 20;
    ostringstream str;
    for (int i=0; i<m; ++i) {
        str << i;
        string s(str.str());
        { Indexable idx(s, 0, writer, 0); }
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
    int m = 2000;
    ostringstream str;
    string size("size");
    for (int i=1; i<=m; ++i) {
        str << i;
        string s(str.str());
        {
             Indexable idx(s, 0, writer, 0);
             idx.setField(size, s);
        }
        str.str("");
    }
    writer->commit();
    Query q("size:>0", -1, 0);
    int count = reader->countHits(q);
    return count != m;
}
/* below here the threading plumbing is done */
void*
threadstarter(void *d) {
    IndexManagerTests* tester = static_cast<IndexManagerTests*>(d);
//    tester->runThreadedTests();
    pthread_exit(0);
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
    pthread_t* thread = new pthread_t[n];
    for (int i=0; i<n; ++i) {
        pthread_create(thread+i, NULL, threadstarter, this);
    }
    for (int i=0; i<n; ++i) {
        pthread_join(thread[i], 0);
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

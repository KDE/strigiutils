#include "indexmanagertests.h"
#include "indexreader.h"
#include "indexwriter.h"
#include "indexmanager.h"
#include <sstream>
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
    int testAll(bool threaded = false);
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
};
int
IndexManagerTester::testAll(bool threaded) {
    int n = 0;

    // tests that only need return 0 when not threaded
    int nt = 0;
    nt += addAndCount();
    if (!threaded) {
        n += nt;
    }

    addErrors(n);
    return n;
}
int
IndexManagerTester::addAndCount() {
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
    printf("%i %i\n", n, m);
    return n != m;
}
/* below here the threading plumbing is done */
void*
threadstarter(void *d) {
    IndexManagerTests* tester = static_cast<IndexManagerTests*>(d);
    tester->testAll(true);
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
IndexManagerTests::testAll(bool threaded) {
    tester->cleanErrors();
    return tester->testAll(threaded);
}

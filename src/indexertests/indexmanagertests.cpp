#include "indexmanagertests.h"
#include "indexreader.h"
#include "indexmanager.h"
using namespace jstreams;

class IndexManagerTester {
private:
    IndexManager* reader;
public:
    IndexManagerTester(IndexManager* w) :reader(w) {}
};

IndexManagerTests::IndexManagerTests(jstreams::IndexManager* m)
    :tester (new IndexManagerTester(m)) {
}
IndexManagerTests::~IndexManagerTests() {
    delete tester;
}
void*
threadstarter(void *d) {
    IndexManagerTests* tester = static_cast<IndexManagerTests*>(d);
    tester->testAll();
    pthread_exit(0);
}
int
IndexManagerTests::testAllInThreads(int n) {
    pthread_t* thread = new pthread_t[n];
    for (int i=0; i<n; ++i) {
        pthread_create(thread+i, NULL, threadstarter, this);
    }
    for (int i=0; i<n; ++i) {
        pthread_join(thread[i], 0);
    }

    delete [] thread;
    return 0;
}

int
IndexManagerTests::testAll() {
    int n = 0;
    return n;
}

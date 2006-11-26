#ifndef INDEXMANAGERTESTS_H
#define INDEXMANAGERTESTS_H

namespace jstreams {
    class IndexManager;
    class IndexerConfiguration;
}

class IndexManagerTester;
class IndexManagerTests {
private:
    IndexManagerTester* tester;
public:
    IndexManagerTests(jstreams::IndexManager* w,
        jstreams::IndexerConfiguration& ic);
    ~IndexManagerTests();
    int testAllInThreads(int n);
    int testAll();
    int runUnthreadedTests();
    int runThreadedTests();
};

#endif

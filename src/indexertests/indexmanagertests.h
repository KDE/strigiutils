#ifndef INDEXMANAGERTESTS_H
#define INDEXMANAGERTESTS_H

#include "verify.h"

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
    void testAllInThreads(int n);
    void testAll();
    void runUnthreadedTests();
    void runThreadedTests();
};

#endif

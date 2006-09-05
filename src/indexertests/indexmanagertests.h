#ifndef INDEXMANAGERTESTS_H
#define INDEXMANAGERTESTS_H

namespace jstreams {
    class IndexManager;
}

class IndexManagerTester;
class IndexManagerTests {
private:
    IndexManagerTester* tester;
public:
    IndexManagerTests(jstreams::IndexManager* w);
    ~IndexManagerTests();
    int testAllInThreads(int n);
    int testAll();
};

#endif

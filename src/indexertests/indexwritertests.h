#ifndef INDEXWRITERTESTS_H
#define INDEXWRITERTESTS_H

namespace jstreams {
    class IndexWriter;
}

class IndexWriterTester;
class IndexWriterTests {
private:
    IndexWriterTester* tester;
public:
    IndexWriterTests(jstreams::IndexWriter* w);
    ~IndexWriterTests();
    int testAll();
};

#endif

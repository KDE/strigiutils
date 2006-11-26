#ifndef INDEXWRITERTESTS_H
#define INDEXWRITERTESTS_H

namespace jstreams {
    class IndexWriter;
    class IndexerConfiguration;
}

class IndexWriterTester;
class IndexWriterTests {
private:
    IndexWriterTester* tester;
public:
    IndexWriterTests(jstreams::IndexWriter& w,
        jstreams::IndexerConfiguration& ic);
    ~IndexWriterTests();
    int testAll();
};

#endif

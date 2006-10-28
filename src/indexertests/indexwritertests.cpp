#include "jstreamsconfig.h"
#include "indexwritertests.h"
#include "indexwriter.h"
using namespace jstreams;

class IndexWriterTester {
private:
    IndexWriter* writer;
public:
    IndexWriterTester(IndexWriter* w) :writer(w) {}
    int optimize() {
        writer->optimize();
        return 0;
    }
    int add() {
        std::string s("a"); // we must pass a string, not a const char*
        {
            Indexable i(s, 0, writer, 0);
        }
        writer->commit();

        return 0;
    }
};

IndexWriterTests::IndexWriterTests(jstreams::IndexWriter* w)
    :tester (new IndexWriterTester(w)) {
}
IndexWriterTests::~IndexWriterTests() {
    delete tester;
}

int
IndexWriterTests::testAll() {
    int n = 0;
    n += tester->add();
    n += tester->optimize();
    return n;
}

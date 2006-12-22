#include "jstreamsconfig.h"
#include "indexreadertests.h"
#include "indexreader.h"
using namespace jstreams;

class IndexReaderTester {
private:
    IndexReader* reader;
public:
    IndexReaderTester(IndexReader* r) :reader(r) {}
    int getFiles(char depth) {
        VERIFY(reader);
        if (reader == 0) return 1;
        reader->getFiles(depth);
        return 0;
    }
};

IndexReaderTests::IndexReaderTests(jstreams::IndexReader* w)
    :tester (new IndexReaderTester(w)) {
}
IndexReaderTests::~IndexReaderTests() {
    delete tester;
}

int
IndexReaderTests::testAll() {
    int n = 0;
    n += tester->getFiles(0);
    return n;
}

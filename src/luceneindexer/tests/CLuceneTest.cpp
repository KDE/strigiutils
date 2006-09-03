#include "jstreamsconfig.h"
#include "cluceneindexmanager.h"
#include "indexwritertests.h"
#include "indexreadertests.h"

int
CLuceneTest(int argc, char**argv) {
    CLuceneIndexManager* manager = new CLuceneIndexManager("test");

    jstreams::IndexWriter* writer = manager->getIndexWriter();
    IndexWriterTests tests(writer);
    int errors = tests.testAll();

    jstreams::IndexReader* reader = manager->getIndexReader();
    IndexReaderTests rtests(reader);
    errors += rtests.testAll();

    delete manager;
    return errors;
}

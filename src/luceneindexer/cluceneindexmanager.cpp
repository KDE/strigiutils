#include "cluceneindexmanager.h"
#include "cluceneindexwriter.h"
#include "cluceneindexreader.h"

CLuceneIndexManager::CLuceneIndexManager(const std::string& path) {
    writer = new CLuceneIndexWriter(path.c_str());
    reader = new CLuceneIndexReader(path.c_str());
}
CLuceneIndexManager::~CLuceneIndexManager() {
    delete writer;
    delete reader;
}
jstreams::IndexReader*
CLuceneIndexManager::getIndexReader() {
    return reader;
}
jstreams::IndexWriter*
CLuceneIndexManager::getIndexWriter() {
    return writer;
}

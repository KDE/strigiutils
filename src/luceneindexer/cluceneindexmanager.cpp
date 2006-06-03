#include "cluceneindexmanager.h"
#include "cluceneindexwriter.h"
#include "cluceneindexreader.h"
#include <CLucene.h>

using namespace lucene::index;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::store::FSDirectory;

pthread_mutex_t CLuceneIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

jstreams::IndexManager*
createCLuceneIndexManager(const char* path) {
    return new CLuceneIndexManager(path);
}

CLuceneIndexManager::CLuceneIndexManager(const std::string& path) {
    dblock = lock;
    dbdir = path;
    indexreader = 0;
    indexwriter = 0;
    writer = new CLuceneIndexWriter(this);
    reader = new CLuceneIndexReader(this);
    analyzer = new StandardAnalyzer();
}
CLuceneIndexManager::~CLuceneIndexManager() {
    // close the writer and analyzer
    delete writer;
    delete reader;
    closeReader();
    closeWriter();
    delete analyzer;
}
jstreams::IndexReader*
CLuceneIndexManager::getIndexReader() {
    return reader;
}
jstreams::IndexWriter*
CLuceneIndexManager::getIndexWriter() {
    return writer;
}
IndexWriter*
CLuceneIndexManager::refWriter() {
    pthread_mutex_lock(&dblock);
    if (indexwriter == 0) {
        closeReader();
        openWriter();
    }
    return indexwriter;
}
void
CLuceneIndexManager::derefWriter() {
    pthread_mutex_unlock(&dblock);
}
IndexReader*
CLuceneIndexManager::refReader() {
    pthread_mutex_lock(&dblock);
    if (indexreader == 0) {
        closeWriter();
        openReader();
    }
    return indexreader;
}
void
CLuceneIndexManager::derefReader() {
    pthread_mutex_unlock(&dblock);
}
void
CLuceneIndexManager::openReader() {
    try {
        indexreader = IndexReader::open(dbdir.c_str());
    } catch (...) {
        printf("could not create reader\n");
    }
}
void
CLuceneIndexManager::closeReader() {
    if (indexreader == 0) return;
    indexreader->close();
    delete indexreader;
    indexreader = 0;
}
void
CLuceneIndexManager::openWriter() {
    try {
        indexwriter = new IndexWriter(dbdir.c_str(), analyzer, true, true);
    } catch (...) {
        printf("could not create writer\n");
    }
}
void
CLuceneIndexManager::closeWriter() {
    if (indexwriter == 0) return;
    indexwriter->optimize();
    indexwriter->close();
    delete indexwriter;
    indexwriter = 0;
}

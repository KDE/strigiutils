#ifndef CLUCENEINDEXMANAGER_H
#define CLUCENEINDEXMANAGER_H

#include "indexmanager.h"
#include <pthread.h>
#include <string>
#include <map>

/**
 **/

namespace lucene {
    namespace analysis {
        class Analyzer;
    }
    namespace index {
        class IndexWriter;
        class IndexReader;
    }
    namespace store {
        class FSDirectory;
    }
}

class CLuceneIndexReader;
class CLuceneIndexWriter;
class CLuceneIndexManager : public jstreams::IndexManager {
private:
    pthread_mutex_t dblock;
    static pthread_mutex_t lock;
    std::string dbdir;
    CLuceneIndexReader* reader;
    CLuceneIndexWriter* writer;
    lucene::index::IndexWriter* indexwriter;
    lucene::index::IndexReader* indexreader;
    lucene::analysis::Analyzer* analyzer;
    int version;
    static int numberOfManagers;

    void openReader();
    void closeReader();
    void openWriter();
    void closeWriter();
public:
    CLuceneIndexManager(const std::string& path);
    ~CLuceneIndexManager();

    lucene::index::IndexWriter* refWriter();
    void derefWriter();
    lucene::index::IndexReader* refReader();
    void derefReader();
    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();
    int docCount();
    int getIndexSize();
    int getVersion() const { return version; }
};

jstreams::IndexManager*
createCLuceneIndexManager(const char* path);

#endif

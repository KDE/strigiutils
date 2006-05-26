#ifndef CLUCENEINDEXMANAGER_H
#define CLUCENEINDEXMANAGER_H

#include "indexmanager.h"
#include <pthread.h>
#include <string>
#include <map>

/**
 **/

class CLuceneIndexReader;
class CLuceneIndexWriter;
class CLuceneIndexManager : public jstreams::IndexManager {
private:
    pthread_mutex_t dblock;
    static pthread_mutex_t lock;
    std::string dbfile;
    std::map<pthread_t, CLuceneIndexReader*> readers;
    std::map<pthread_t, CLuceneIndexWriter*> writers;
public:
    CLuceneIndexManager(const char* path);
    ~CLuceneIndexManager();
    const char* getDBFile() const { return dbfile.c_str(); }

    void ref();
    void deref();
    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();
    static std::string escapeSqlValue(const std::string& value);
};

#endif

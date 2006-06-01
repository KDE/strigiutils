#ifndef ESTRAIERINDEXMANAGER_H
#define ESTRAIERINDEXMANAGER_H

#include "indexmanager.h"
#include <estraier.h>
#include <pthread.h>
#include <string>
#include <map>

/**
 * Manage the database connection between threads.
 * http://www.estraier.org/cvstrac/wiki?p=MultiThreading
 **/

class EstraierIndexReader;
class EstraierIndexWriter;
class EstraierIndexManager : public jstreams::IndexManager {
private:
    pthread_mutex_t dblock;
    static pthread_mutex_t lock;
    std::string dbdir;
    bool writing;
    ESTDB* db;
    std::map<pthread_t, EstraierIndexReader*> readers;
    std::map<pthread_t, EstraierIndexWriter*> writers;

    void closedb();
public:
    EstraierIndexManager(const char* path);
    ~EstraierIndexManager();

    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();
    void ref();
    void deref();
};

#endif

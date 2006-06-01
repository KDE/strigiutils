#ifndef XAPIANINDEXMANAGER_H
#define XAPIANINDEXMANAGER_H

#include "indexmanager.h"
#include <xapian.h>
#include <pthread.h>
#include <string>
#include <map>

/**
 * Manage the database connection between threads.
 * http://www.xapian.org/cvstrac/wiki?p=MultiThreading
 **/

class XapianIndexReader;
class XapianIndexWriter;
class XapianIndexManager : public jstreams::IndexManager {
private:
    pthread_mutex_t dblock;
    static pthread_mutex_t lock;
    std::string dbdir;
    bool writing;
    Xapian::WritableDatabase* db;
    std::map<pthread_t, XapianIndexReader*> readers;
    std::map<pthread_t, XapianIndexWriter*> writers;

    void closedb();
public:
    XapianIndexManager(const char* path);
    ~XapianIndexManager();

    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();
    void ref();
    void deref();
};

#endif

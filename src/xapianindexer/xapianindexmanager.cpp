#include "xapianindexmanager.h"
#include "xapianindexreader.h"
#include "xapianindexwriter.h"
#include <assert.h>
using namespace std;
using namespace jstreams;
using namespace Xapian;

pthread_mutex_t XapianIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

XapianIndexManager::XapianIndexManager(const char* dbd)
        : dblock(lock), dbdir(dbd) {
    db = new WritableDatabase(dbdir.c_str(), DB_CREATE_OR_OPEN);
}
XapianIndexManager::~XapianIndexManager() {
    std::map<pthread_t, XapianIndexReader*>::iterator r;
    for (r = readers.begin(); r != readers.end(); ++r) {
        delete r->second;
    }
    std::map<pthread_t, XapianIndexWriter*>::iterator w;
    for (w = writers.begin(); w != writers.end(); ++w) {
        delete w->second;
    }
    closedb();
}
IndexReader*
XapianIndexManager::getIndexReader() {
    pthread_t self = pthread_self();
    XapianIndexReader* r = readers[self];
    if (r == 0) {
        r = new XapianIndexReader(this, db);
        readers[self] = r;
    }
    return r;
}
IndexWriter*
XapianIndexManager::getIndexWriter() {
    pthread_t self = pthread_self();
    XapianIndexWriter* w = writers[self];
    if (w == 0) {
        w = new XapianIndexWriter(this, db);
        writers[self] = w;
    }
    return w;
}
/*ESTDB*
XapianIndexManager::getWriteDB() {
    pthread_mutex_lock(&dblock);
    // check if a writable db is already opened
    if (db) {
        if (writing) return db;
        closedb();
    }
    writing = true;
    int errorcode;
    db = est_db_open(dbdir.c_str(), ESTDBWRITER|ESTDBCREAT, &errorcode);
    if (db == 0) printf("could not open db: error %i\n", errorcode);
    return db;
}
void
XapianIndexManager::returnWriteDB() {
    pthread_mutex_unlock(&dblock);
}
ESTDB*
XapianIndexManager::getReadDB() {
    pthread_mutex_lock(&dblock);
    // check if a readable db is already opened
    if (db) {
        if (!writing) return db;
        closedb();
    }
    writing = false;
    return db;
}
void
XapianIndexManager::returnReadDB() {
    pthread_mutex_unlock(&dblock);
}*/
void
XapianIndexManager::ref() {
    pthread_mutex_lock(&dblock);
}
void
XapianIndexManager::deref() {
    pthread_mutex_unlock(&dblock);
}
void
XapianIndexManager::closedb() {
    if (db == 0) return;
    delete db;
    db = 0;
}

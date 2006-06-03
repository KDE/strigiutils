#include "estraierindexmanager.h"
#include "estraierindexreader.h"
#include "estraierindexwriter.h"
#include <assert.h>
using namespace std;
using namespace jstreams;

pthread_mutex_t EstraierIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

jstreams::IndexManager*
createEstraierIndexManager(const char* path) {
    return new EstraierIndexManager(path);
}

EstraierIndexManager::EstraierIndexManager(const char* dbd)
        : dblock(lock), dbdir(dbd) {
    int errorcode;
    db = est_db_open(dbdir.c_str(), ESTDBWRITER|ESTDBCREAT, &errorcode);
    if (db == 0) printf("could not open db %s: %s\n", dbdir.c_str(),
        est_err_msg(errorcode));
}
EstraierIndexManager::~EstraierIndexManager() {
    std::map<pthread_t, EstraierIndexReader*>::iterator r;
    for (r = readers.begin(); r != readers.end(); ++r) {
        delete r->second;
    }
    std::map<pthread_t, EstraierIndexWriter*>::iterator w;
    for (w = writers.begin(); w != writers.end(); ++w) {
        delete w->second;
    }
    closedb();
}
IndexReader*
EstraierIndexManager::getIndexReader() {
    pthread_t self = pthread_self();
    EstraierIndexReader* r = readers[self];
    if (r == 0) {
        r = new EstraierIndexReader(this, db);
        readers[self] = r;
    }
    return r;
}
IndexWriter*
EstraierIndexManager::getIndexWriter() {
    pthread_t self = pthread_self();
    EstraierIndexWriter* w = writers[self];
    if (w == 0) {
        w = new EstraierIndexWriter(this, db);
        writers[self] = w;
    }
    return w;
}
/*ESTDB*
EstraierIndexManager::getWriteDB() {
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
EstraierIndexManager::returnWriteDB() {
    pthread_mutex_unlock(&dblock);
}
ESTDB*
EstraierIndexManager::getReadDB() {
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
EstraierIndexManager::returnReadDB() {
    pthread_mutex_unlock(&dblock);
}*/
void
EstraierIndexManager::ref() {
    pthread_mutex_lock(&dblock);
}
void
EstraierIndexManager::deref() {
    pthread_mutex_unlock(&dblock);
}
void
EstraierIndexManager::closedb() {
    if (db == 0) return;
    int errorcode;
    int ok = est_db_close(db, &errorcode);
    if (!ok) printf("could not close db: error %i\n", errorcode);
    db = 0;
}

#include "cluceneindexmanager.h"
#include "cluceneindexwriter.h"
#include "cluceneindexreader.h"
#include <CLucene.h>
#include <sys/types.h>
#include <dirent.h>

using namespace lucene::index;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::store::FSDirectory;

pthread_mutex_t CLuceneIndexManager::lock = PTHREAD_MUTEX_INITIALIZER;

jstreams::IndexManager*
createCLuceneIndexManager(const char* path) {
    return new CLuceneIndexManager(path);
}

int CLuceneIndexManager::numberOfManagers = 0;

CLuceneIndexManager::CLuceneIndexManager(const std::string& path) {
    ++numberOfManagers;
    dblock = lock;
    dbdir = path;
    indexreader = 0;
    indexwriter = 0;
    version = 0;
    writer = new CLuceneIndexWriter(this);
    reader = new CLuceneIndexReader(this);
    analyzer = new StandardAnalyzer();
    // make sure there's at least an index
    openWriter();
}
CLuceneIndexManager::~CLuceneIndexManager() {
    // close the writer and analyzer
    delete writer;
    delete reader;
    closeReader();
    closeWriter();
    delete analyzer;
    if (--numberOfManagers == 0) {
// temporarily commented out because of problem with clucene
//        _lucene_shutdown();
    }
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
//        printf("reader at %s\n", dbdir.c_str());
        indexreader = IndexReader::open(dbdir.c_str());
    } catch (CLuceneError& err) {
        printf("could not create reader: %s\n", err.what());
    }
}
void
CLuceneIndexManager::closeReader() {
    if (indexreader == 0) return;
    try {
        indexreader->close();
    } catch (CLuceneError& err) {
        printf("could not close clucene: %s\n", err.what());
    }
    delete indexreader;
    indexreader = 0;
}
void
CLuceneIndexManager::openWriter() {
    version++;
    try {
//        printf("writer at %s\n", dbdir.c_str());
        if (IndexReader::indexExists(dbdir.c_str())) {
            if (IndexReader::isLocked(dbdir.c_str())) {
                IndexReader::unlock(dbdir.c_str());
            }
            indexwriter = new IndexWriter(dbdir.c_str(), analyzer, false);
        } else {
            indexwriter = new IndexWriter(dbdir.c_str(), analyzer, true, true);
        }
    } catch (CLuceneError& err) {
        printf("could not create writer: %s\n", err.what());
    }
}
void
CLuceneIndexManager::closeWriter() {
    if (indexwriter == 0) return;
    indexwriter->close();
    delete indexwriter;
    indexwriter = 0;
}
int
CLuceneIndexManager::docCount() {
    int count;
    pthread_mutex_lock(&dblock);
    if (indexwriter) {
        count = indexwriter->docCount();
    } else {
        if (indexreader == 0) {
            openReader();
        }
        count = indexreader->numDocs();
    }
    pthread_mutex_unlock(&dblock);
    return count;
}
int
CLuceneIndexManager::getIndexSize() {
    // sum the sizes of the files in the index
    // loop over directory entries
    DIR* dir = opendir(dbdir.c_str());
    if (dir == 0) {
        fprintf(stderr, "could not open index directory.\n");
        return -1;
    }
    struct dirent* e = readdir(dir);
    int size = 0;
    while (e != 0) {
        string filename = dbdir+"/"+e->d_name;
        struct stat s;
        int r = stat(filename.c_str(), &s);
        if (r == 0) {
            if (S_ISREG(s.st_mode)) {
                size += s.st_size;
            }
        } else {
            fprintf(stderr, "could not open file %s\n", filename.c_str());
        }
        e = readdir(dir);
    }
    closedir(dir);
    return size;
}

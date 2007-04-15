#include "grepindexmanager.h"

GrepIndexManager::GrepIndexManager(const char* path) {
}
GrepIndexManager::~GrepIndexManager() {
}
Strigi::IndexReader*
GrepIndexManager::indexReader() {
    return 0;
}
Strigi::IndexWriter*
GrepIndexManager::indexWriter() {
    return 0;
}
Strigi::IndexManager*
createGrepIndexManager(const char* path) {
    return new GrepIndexManager(path);
}

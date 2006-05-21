#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

namespace jstreams {
class IndexReader;
class IndexWriter;
class IndexManager {
public:
    virtual ~IndexManager() {}
    virtual IndexReader* getIndexReader() = 0;
    virtual IndexWriter* getIndexWriter() = 0;
};
}

#endif

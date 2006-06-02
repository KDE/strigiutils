#ifndef CLUCENEINDEXREADER_H
#define CLUCENEINDEXREADER_H

#include "indexreader.h"
#include <map>

class CLuceneIndexManager;
class CLuceneIndexReader : public jstreams::IndexReader {
friend class CLuceneIndexManager;
private:
    CLuceneIndexManager* manager;
    CLuceneIndexReader(CLuceneIndexManager* m) :manager(m) {}
    ~CLuceneIndexReader();
public:
    std::vector<jstreams::IndexedDocument> query(const std::string&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
};

#endif

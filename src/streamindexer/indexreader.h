#ifndef INDEXREADER_H
#define INDEXREADER_H

#include <string>
#include <vector>
#include <map>

namespace jstreams {

struct IndexedDocument {
    std::string filepath;
    std::string title;
    std::string fragment;
};

class IndexReader {
public:
    virtual ~IndexReader() {}
    virtual std::vector<IndexedDocument> query(const std::string&) = 0;
    virtual std::map<std::string, time_t> getFiles(char depth) = 0;
    virtual int countDocuments() { return -1; }
    virtual int countWords() { return -1; }
    virtual int getIndexSize() { return -1; }
};

}

#endif

#ifndef INDEXREADER_H
#define INDEXREADER_H

#include <string>
#include <vector>
#include <map>
#include <set>

namespace jstreams {

class IndexedDocument {
public:
    IndexedDocument() :score(0) {}
    std::string filepath;
    std::string fragment;
    float score;
    std::map<std::string, std::string> properties;
};

/**
 * Break up a string in a query.
 * Currently very simple. Currently always combines terms with AND.
 **/
class Query {
private:
    std::map<std::string, std::set<std::string> > includes;
    std::map<std::string, std::set<std::string> > excludes;

    const char* parseTerm(const char*);
public:
    Query(const std::string& q);
    const std::map<std::string, std::set<std::string> > &getIncludes() const {
        return includes;
    }
    const std::map<std::string, std::set<std::string> > &getExcludes() const {
        return excludes;
    }
};

class IndexReader {
public:
    virtual ~IndexReader() {}
    virtual int countHits(const Query&) = 0;
    virtual std::vector<IndexedDocument> query(const Query&) = 0;
    virtual std::map<std::string, time_t> getFiles(char depth) = 0;
    virtual int countDocuments() { return -1; }
    virtual int countWords() { return -1; }
    virtual int getIndexSize() { return -1; }
};

}

#endif

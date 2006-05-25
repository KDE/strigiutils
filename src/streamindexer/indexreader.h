#ifndef INDEXREADER_H
#define INDEXREADER_H

#include <string>
#include <vector>
#include <map>

namespace jstreams {

class IndexReader {
public:
    virtual ~IndexReader() {}
    virtual std::vector<std::string> query(const std::string&) = 0;
    virtual std::map<std::string, time_t> getFiles(char depth) = 0;
    virtual int countDocuments() = 0;
};

}

#endif

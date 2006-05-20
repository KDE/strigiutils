#ifndef INDEXREADER_H
#define INDEXREADER_H

#include <string>
#include <vector>

namespace jstreams {

class IndexReader {
public:
    virtual ~IndexReader() {}
    virtual std::vector<std::string> query(const std::string&) = 0;
};

}

#endif

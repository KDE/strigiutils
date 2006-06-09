#ifndef INDEXEDDOCUMENT_H
#define INDEXEDDOCUMENT_H

#include <map>
#include <string>

namespace jstreams {

class IndexedDocument {
public:
    IndexedDocument() :score(0), size(-1), mtime(0) {}
    std::string uri;
    float score;
    std::string fragment;
    std::string mimetype;
    std::string sha1;
    int64_t size;
    time_t mtime;
    std::map<std::string, std::string> properties;
};

}

#endif

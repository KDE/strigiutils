#ifndef SUBSTREAMPROVIDER
#define SUBSTREAMPROVIDER

#include <string>

#include "subinputstream.h"

namespace jstreams {

struct EntryInfo {
    std::string filename;
    int32_t size;
    uint mtime;
    enum Type {Dir, File};
    Type type;
};

class SubStreamProvider {
protected:
    StreamStatus status;
    std::string error;
    InputStream *input;
    EntryInfo entryinfo;
public:
    SubStreamProvider(InputStream *i) :status(Ok), input(i) {}
    virtual ~SubStreamProvider() {}
    StreamStatus getStatus() const { return status; }
    virtual SubInputStream* nextEntry() = 0;
    const EntryInfo &getEntryInfo() const {
        return entryinfo;
    }
//    std::string getEntryFileName() const { return entryfilename; }
    const char* getError() const { return error.c_str(); }
};

} // end namespace jstreams

#endif

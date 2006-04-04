#ifndef SUBSTREAMPROVIDER
#define SUBSTREAMPROVIDER

#include <string>

class InputStream;
class SubInputStream;

struct EntryInfo {
    enum Type {Dir, File};
    Type type;
    std::string filename;
};

class SubStreamProvider {
protected:
    char status;
    std::string error;
    InputStream *input;
    EntryInfo entryinfo;
public:
    SubStreamProvider(InputStream *input) {
        this->input = input; 
        status = 0;
    }
    virtual ~SubStreamProvider() {}
    virtual SubInputStream* nextEntry() = 0;
    const EntryInfo &getEntryInfo() const {
        return entryinfo;
    }
//    std::string getEntryFileName() const { return entryfilename; }
    std::string getError() const { return error; }
};

#endif

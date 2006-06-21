#ifndef DUMMYINDEXWRITER_H
#define DUMMYINDEXWRITER_H

#include "indexwriter.h"

class DummyIndexWriter : public jstreams::IndexWriter {
private:
    int verbosity;
protected:
    void startIndexable(jstreams::Indexable*i) {
        if (verbosity >= 1) {
            printf("%s\n", i->getName().c_str());
        }
    }
    void finishIndexable(const jstreams::Indexable*) {}
    void addText(const jstreams::Indexable* idx, const char* text,
        int32_t length) {
        if (verbosity > 2) {
            printf("%s: addText '%s'\n", idx->getName().c_str(), text);
        }
    }
    void setField(const jstreams::Indexable* idx, const std::string &fieldname,
            const std::string& value) {
        if (verbosity > 1) {
            printf("%s: setField '%s': '%s'\n", idx->getName().c_str(),
                fieldname.c_str(), value.c_str());
        }
    }
public:
    DummyIndexWriter(int v = 0) {
        verbosity = v;
    }
    ~DummyIndexWriter() {}
    void commit() {}
    void deleteEntries(const std::vector<std::string>& entries) {}
};

#endif

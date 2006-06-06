#ifndef DUMMYINDEXWRITER_H
#define DUMMYINDEXWRITER_H

#include "indexwriter.h"

class DummyIndexWriter : public jstreams::IndexWriter {
protected:
    void startIndexable(jstreams::Indexable*i) {
//        printf("%s\n",i->getName().c_str());
    }
    void finishIndexable(const jstreams::Indexable*) {}
    void addText(const jstreams::Indexable* idx, const char* text,
        int32_t length) {}
    void setField(const jstreams::Indexable* idx, const std::string &fieldname,
        const std::string& value) {}
public:
    DummyIndexWriter() {}
    ~DummyIndexWriter() {}
    void commit() {}
    void deleteEntries(const std::vector<std::string>& entries) {}
};

#endif

#ifndef DUMMYINDEXWRITER_H
#define DUMMYINDEXWRITER_H

#include "indexwriter.h"

class DummyIndexWriter : public jstreams::IndexWriter {
protected:
    void startIndexable(jstreams::Indexable*) {}
    void finishIndexable(const jstreams::Indexable*) {}
    void addStream(const jstreams::Indexable*, const std::string& fieldname,
        jstreams::StreamBase<wchar_t>* datastream) {}
    void addField(const jstreams::Indexable* idx, const std::string &fieldname,
        const std::string& value) {}
    void setField(const jstreams::Indexable*, const std::string &fieldname,
        int64_t value) {}
public:
    DummyIndexWriter() {}
    ~DummyIndexWriter() {}
    void commit() {}
    void deleteEntry(const std::string& entry) {};
};

#endif

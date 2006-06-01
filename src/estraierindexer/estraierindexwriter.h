#ifndef ESTRAIERINDEXWRITER_H
#define ESTRAIERINDEXWRITER_H

#include "indexwriter.h"
#include <estraier.h>

class EstraierIndexManager;
class EstraierIndexWriter : public jstreams::IndexWriter {
friend class EstraierIndexManager;
private:
    EstraierIndexManager* manager;
    const std::string indexpath;
    ESTDB* db;

protected:
    void startIndexable(jstreams::Indexable*);
    void finishIndexable(const jstreams::Indexable*);
    void addStream(const jstreams::Indexable*, const std::string& fieldname,
        jstreams::StreamBase<wchar_t>* datastream);
    void addField(const jstreams::Indexable* idx, const std::string &fieldname,
        const std::string& value);
    void setField(const jstreams::Indexable*, const std::string &fieldname,
        int64_t value);
    EstraierIndexWriter(EstraierIndexManager*, ESTDB*);
    ~EstraierIndexWriter();
public:
    void commit();
    void deleteEntries(const std::vector<std::string>& entries);
    void deleteEntry(const std::string& entry);
    int itemsInCache() { return 0; };
};

#endif

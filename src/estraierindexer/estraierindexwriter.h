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
    void addText(const jstreams::Indexable*, const char* text,
        int32_t length);
    void setField(const jstreams::Indexable* idx, const std::string &fieldname,
        const std::string& value);
    EstraierIndexWriter(EstraierIndexManager*, ESTDB*);
    ~EstraierIndexWriter();
public:
    void commit();
    void deleteEntries(const std::vector<std::string>& entries);
    int itemsInCache() { return 0; };
};

#endif

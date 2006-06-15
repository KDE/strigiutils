#ifndef ESTRAIERINDEXREADER_H
#define ESTRAIERINDEXREADER_H

#include "indexreader.h"
#include <map>
#include <estraier.h>

class EstraierIndexManager;

class EstraierIndexReader : public jstreams::IndexReader {
friend class EstraierIndexManager;
private:
    EstraierIndexManager* manager;
    ESTDB* db;
    EstraierIndexReader(EstraierIndexManager* m, ESTDB* d);
    ~EstraierIndexReader();
    std::string getFragment(ESTDOC* doc, const jstreams::Query& query);
    static ESTCOND* createCondition(const jstreams::Query&);
    static const char* mapId(const std::string& id);
public:
    int32_t countHits(const jstreams::Query&);
    std::vector<jstreams::IndexedDocument> query(const jstreams::Query&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
    int32_t countWords();
    int64_t getIndexSize();
};

#endif

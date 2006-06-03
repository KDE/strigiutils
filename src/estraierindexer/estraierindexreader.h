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
    static ESTCOND* createCondition(const jstreams::Query&);
    static const char* mapId(const std::string& id);
public:
    std::vector<jstreams::IndexedDocument> query(const jstreams::Query&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
    int countWords();
    int getIndexSize();
};

#endif

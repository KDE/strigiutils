#ifndef SQLITEINDEXWRITER_H
#define SQLITEINDEXWRITER_H

#include "indexwriter.h"
#include <vector>
#include <map>

class SqliteIndexWriter : public jstreams::IndexWriter {
private:
    const std::string indexpath;
protected:
    void startIndexable(const jstreams::Indexable*);
    void finishIndexable(const jstreams::Indexable*);
    void addStream(const jstreams::Indexable*, const std::wstring& fieldname,
        jstreams::StreamBase<wchar_t>* datastream);
    void addField(const jstreams::Indexable*, const std::wstring &fieldname,
        const char* value);
    void addField(const jstreams::Indexable*, const std::wstring &fieldname,
        const std::wstring &value);
public:
    SqliteIndexWriter(const char* path);
    ~SqliteIndexWriter();
};

#endif

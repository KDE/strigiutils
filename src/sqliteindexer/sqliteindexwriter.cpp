#include "sqliteindexwriter.h"

using namespace std;
using namespace jstreams;

SqliteIndexWriter::SqliteIndexWriter(const char* ip) :indexpath(ip) {
}
SqliteIndexWriter::~SqliteIndexWriter() {
}
void
SqliteIndexWriter::addStream(const Indexable* idx, const wstring& fieldname,
        StreamBase<wchar_t>* datastream) {
}
/*
   If there's an open document, add fields to that. If there isn't open one.
*/
void
SqliteIndexWriter::addField(const Indexable* idx, const wstring &fieldname,
        const wstring &value) {
}
void
SqliteIndexWriter::addField(const Indexable* idx, const wstring &fieldname,
        const char* value) {
}

void
SqliteIndexWriter::startIndexable(const Indexable* idx) {
    addField(idx, L"path", idx->getName().c_str());
}
/*
    Close all left open indexwriters for this path.
*/
void
SqliteIndexWriter::finishIndexable(const Indexable* idx) {
}

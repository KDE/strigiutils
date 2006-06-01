#include "xapianindexwriter.h"
#include "xapianindexmanager.h"
#include <vector>
#include <sstream>
using namespace std;
using namespace jstreams;
using namespace Xapian;

XapianIndexWriter::XapianIndexWriter(XapianIndexManager *m,
    Xapian::WritableDatabase* d) : manager(m), db(d) {
}
XapianIndexWriter::~XapianIndexWriter() {
}
void
XapianIndexWriter::addStream(const Indexable* idx, const string& fieldname,
        StreamBase<wchar_t>* datastream) {
}
void
XapianIndexWriter::addField(const Indexable* idx, const string& name,
        const string& value) {
}
void
XapianIndexWriter::setField(const Indexable* idx, const std::string& name,
        int64_t value) {
}

void
XapianIndexWriter::startIndexable(Indexable* idx) {
    Document *doc = new Document();
    idx->setWriterData(doc);
}
/*
    Close all left open indexwriters for this path.
*/
void
XapianIndexWriter::finishIndexable(const Indexable* idx) {
    static const string path("path"), mtime("mtime"), depth("depth");
    Document* doc = static_cast<Document*>(idx->getWriterData());
    manager->ref();
    db->add_document(*doc);
    manager->deref();
    delete doc;
}
void
XapianIndexWriter::commit() {
}
/**
 * Delete all files that start with the specified path.
 **/
void
XapianIndexWriter::deleteEntries(const std::vector<std::string>& entries) {
}
void
XapianIndexWriter::deleteEntry(const string& path) {
}

#include "estraierindexwriter.h"
#include "estraierindexmanager.h"
#include <vector>
#include <sstream>
using namespace std;
using namespace jstreams;

EstraierIndexWriter::EstraierIndexWriter(EstraierIndexManager *m, ESTDB* d)
        : manager(m), db(d) {
}
EstraierIndexWriter::~EstraierIndexWriter() {
    // make sure the cache is empty
    commit();
}
void
EstraierIndexWriter::addStream(const Indexable* idx, const string& fieldname,
        StreamBase<wchar_t>* datastream) {
}
void
EstraierIndexWriter::addField(const Indexable* idx, const string& name,
        const string& value) {
    ESTDOC* doc = (ESTDOC*)idx->getWriterData();
    if (name == "content") {
        est_doc_add_text(doc, value.c_str());
    } else {
        est_doc_add_attr(doc, name.c_str(), value.c_str());
    }
}
void
EstraierIndexWriter::setField(const Indexable* idx, const std::string& name,
        int64_t value) {
}

void
EstraierIndexWriter::startIndexable(Indexable* idx) {
    // allocate a new estraier document
    ESTDOC* doc = est_doc_new();
    idx->setWriterData(doc);
}
/*
    Close all left open indexwriters for this path.
*/
void
EstraierIndexWriter::finishIndexable(const Indexable* idx) {
    ESTDOC* doc = static_cast<ESTDOC*>(idx->getWriterData());
    // add required url field
    est_doc_add_attr(doc, "@uri", idx->getName().c_str());
    char numbuf[64];
    sprintf(numbuf, "%lli", idx->getMTime());
    est_doc_add_attr(doc, "@mdate", numbuf);
    sprintf(numbuf, "%i", idx->getDepth());
    est_doc_add_attr(doc, "depth", numbuf);

    manager->ref();
    int ok = est_db_put_doc(db, doc, 0);
    if (!ok) printf("could not write document\n");
    // deallocate the estraier document
    est_doc_delete(doc);
    manager->deref();
}
void
EstraierIndexWriter::commit() {
}
/**
 * Delete all files that start with the specified path.
 **/
void
EstraierIndexWriter::deleteEntries(const std::vector<std::string>& entries) {
    vector<string>::const_iterator i;
    for (i = entries.begin(); i != entries.end(); ++i) {
        deleteEntry(*i);
    }
}
void
EstraierIndexWriter::deleteEntry(const string& path) {
    ESTCOND* cond = est_cond_new();
    string q = "@uri STRBW "+path;
    est_cond_add_attr(cond, q.c_str());
    int n;
    int* ids;
    manager->ref();
    ids = est_db_search(db, cond, &n, NULL);
    
    for (int i=0; i<n; ++i) {
        est_db_out_doc(db, ids[i], 0);
    }
    manager->deref();

    // clean up
    est_cond_delete(cond);
    free(ids);
}

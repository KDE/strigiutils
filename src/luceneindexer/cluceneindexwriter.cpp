#include "cluceneindexwriter.h"
#include "cluceneindexmanager.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>
#include "stringreader.h"
#include "inputstreamreader.h"
#include <assert.h>

using lucene::index::IndexWriter;
using lucene::analysis::Analyzer;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::document::Document;
using lucene::document::Field;
using lucene::util::Reader;
using namespace std;
using namespace jstreams;

struct CLuceneDocData {
    lucene::document::Document doc;
    std::string content;
};
CLuceneIndexWriter::~CLuceneIndexWriter() {
}
void
CLuceneIndexWriter::addStream(const Indexable* idx,
        const string& fieldname, StreamBase<wchar_t>* datastream) {
    // i'm rethinking how to feed streams
    assert(0);
/*    Document *doc = docs[idx];
    if (doc == 0) {
        doc = new Document();
    } else {
        docs[idx] = 0;
    }
    Reader* reader = new Reader(datastream, false);
#if defined(_UCS2)
    TCHAR fn[CL_MAX_DIR];
    STRCPY_AtoT(fn, fieldname.c_str(), CL_MAX_DIR);
    doc->add( *Field::Text(fn, reader) );
#endif
    if (writers.size() < activewriter+1) {
        addIndexWriter();
    }
    lucene::index::IndexWriter* writer = writers[activewriter];
    activewriter++;
    try {
        writer->addDocument(doc);
    } catch (...) {
        // empty catch: the exception is the result of the document encoding
        // not being correct
    }

    delete doc;
    activewriter--;*/
}
/*
   If there's an open document, add fields to that. If there isn't open one.
*/
void
CLuceneIndexWriter::addField(const Indexable* idx, const string& fieldname,
        const string& value) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->getWriterData());
    if (fieldname == "content") {
        doc->content += value + " ";
        return;
    }
#if defined(_UCS2)
    TCHAR fn[CL_MAX_DIR];
    TCHAR fv[CL_MAX_DIR];
    STRCPY_AtoT(fn, fieldname.c_str(), CL_MAX_DIR);
    STRCPY_AtoT(fv, value.c_str(), CL_MAX_DIR);
    doc->doc.add( *Field::Keyword(fn, fv) );
#else
    doc->doc.add(*Field::Keyword(fieldname.c_str(), value.c_str()));
#endif
}
void
CLuceneIndexWriter::setField(const Indexable*, const std::string &fieldname,
        int64_t value) {
}
void
CLuceneIndexWriter::startIndexable(Indexable* idx) {
    doccount++;
    CLuceneDocData*doc = new CLuceneDocData();
    idx->setWriterData(doc);
}
/*
    Close all left open indexwriters for this path.
*/
void
CLuceneIndexWriter::finishIndexable(const Indexable* idx) {
    addField(idx, "path", idx->getName());
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->getWriterData());
    if (doc->content.length() > 0) {
#if defined(_UCS2)
        StringReader<char> sr(doc->content.c_str(), doc->content.length(),
            false);
        InputStreamReader streamreader(&sr);
        Reader* reader = new Reader(&streamreader, false);
        doc->doc.add( *Field::Text(L"content", reader) );
#else
        doc->doc.add(*Field::Keyword("content", doc->content.c_str()));
#endif
        lucene::index::IndexWriter* writer = manager->refWriter();
        writer->addDocument(&doc->doc);
    } else {
        lucene::index::IndexWriter* writer = manager->refWriter();
        writer->addDocument(&doc->doc);
    }
    manager->derefWriter();
    delete doc;
}

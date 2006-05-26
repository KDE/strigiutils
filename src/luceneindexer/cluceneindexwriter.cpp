#include "cluceneindexwriter.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>
#include "stringreader.h"
#include "inputstreamreader.h"
#include <assert.h>

using namespace std;
using namespace jstreams;
using lucene::analysis::Analyzer;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::document::Document;
using lucene::document::Field;
using lucene::util::Reader;

CLuceneIndexWriter::CLuceneIndexWriter(const char* ip) :indexpath(ip) {
    analyzer = new StandardAnalyzer();
    writer = new lucene::index::IndexWriter( indexpath.c_str(), analyzer, 1);
    doccount = 0;
}
CLuceneIndexWriter::~CLuceneIndexWriter() {
    // close the writer and analyzer
    writer->optimize();
    writer->close();
    delete writer;
    delete analyzer;
}
void
CLuceneIndexWriter::addStream(const Indexable* idx, const string& fieldname,
        StreamBase<wchar_t>* datastream) {
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
    if (fieldname == "content") {
        content[idx] += value + " ";
        return;
    }
#if defined(_UCS2)
    TCHAR fn[CL_MAX_DIR];
    TCHAR fv[CL_MAX_DIR];
    STRCPY_AtoT(fn, fieldname.c_str(), CL_MAX_DIR);
    STRCPY_AtoT(fv, value.c_str(), CL_MAX_DIR);
    docs[idx].add( *Field::Keyword(fn, fv) );
#else
    docs[idx]add(*Field::Keyword(fieldname.c_str(), value.c_str()));
#endif
}
void
CLuceneIndexWriter::setField(const Indexable*, const std::string &fieldname,
        int64_t value) {
}
void
CLuceneIndexWriter::startIndexable(Indexable* idx) {
    doccount++;
    addField(idx, "path", idx->getName());
}
/*
    Close all left open indexwriters for this path.
*/
void
CLuceneIndexWriter::finishIndexable(const Indexable* idx) {
    std::map<const jstreams::Indexable*, string>::iterator c
        = content.find(idx);
    std::map<const jstreams::Indexable*, lucene::document::Document>::iterator i
        = docs.find(idx);
    if (c != content.end()) {
#if defined(_UCS2)
        StringReader<char> sr(c->second.c_str(), c->second.length(), false);
        InputStreamReader streamreader(&sr);
//        Reader* reader = new Reader(&streamreader, false);
//        i->second.add( *Field::Text(L"content", reader) );
#else
        i->second.add(*Field::Keyword("content", c->second.c_str()));
#endif
        content.erase(c);
    }
    writer->addDocument(&i->second);
    docs.erase(i);
}

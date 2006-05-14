#include "cluceneindexwriter.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>

using namespace std;
using namespace jstreams;
using lucene::analysis::Analyzer;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::document::Document;
using lucene::document::Field;
using lucene::util::Reader;

CLuceneIndexWriter::CLuceneIndexWriter(const char* ip) :indexespath(ip) {
    activewriter = 0;
}
CLuceneIndexWriter::~CLuceneIndexWriter() {
    // close all writers and analyzers
    for (int i=0; i<writers.size(); ++i) {
        lucene::index::IndexWriter* writer = writers[i];
	writer->optimize();
        writer->close();
	delete writer;
        Analyzer* analyzer = analyzers[i];
	delete analyzer;
    }
}
void
CLuceneIndexWriter::addIndexWriter() {
    Analyzer* analyzer = new StandardAnalyzer();
    analyzers.push_back(analyzer);
    // check if the required directory exists
    string dir = indexespath+(char)('1'+activewriter);
    mkdir(dir.c_str(), 0777);
    lucene::index::IndexWriter* writer = new lucene::index::IndexWriter(
        dir.c_str(), analyzer, 1);
    writers.push_back(writer);
}
void
CLuceneIndexWriter::addStream(const Indexable* idx, const wstring& fieldname,
        StreamBase<wchar_t>* datastream) {
    Document *doc = docs[idx];
    if (doc == 0) {
        doc = new Document();
    } else {
        docs[idx] = 0;
    }
    Reader* reader = new Reader(datastream, false);
    doc->add( *Field::Text(fieldname.c_str(), reader) );

    if (writers.size() < activewriter+1) {
        addIndexWriter();
    }
    lucene::index::IndexWriter* writer = writers[activewriter];
    activewriter++;
    try {
        writer->addDocument(doc);
    } catch (...) {
    }

    delete doc;
    activewriter--;
}
/*
   If there's an open document, add fields to that. If there isn't open one.
*/
void
CLuceneIndexWriter::addField(const Indexable* idx, const wstring &fieldname,
        const wstring &value) {
    Document *doc = docs[idx];
    if (doc == 0) {
        doc = new Document();
        docs[idx] = doc;
    }
    doc->add( *Field::Keyword(fieldname.c_str(), value.c_str()) );
}
void
CLuceneIndexWriter::addField(const Indexable* idx, const wstring &fieldname,
        const char* value) {
    TCHAR tf[CL_MAX_DIR];
    STRCPY_AtoT(tf, value, CL_MAX_DIR);
    wstring v(tf);
    addField(idx, fieldname, v);
}

void
CLuceneIndexWriter::startIndexable(const Indexable* idx) {
    addField(idx, L"path", idx->getName().c_str());
}
/*
    Close all left open indexwriters for this path.
*/
void
CLuceneIndexWriter::finishIndexable(const Indexable* idx) {
    Document *doc = docs[idx];
    if (doc) {
        if (writers.size() < activewriter+1) {
            addIndexWriter();
        }
        writers[activewriter]->addDocument(doc);
        delete doc;
        docs[idx] = 0;
    }
}

#include "cluceneindexwriter.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>

using namespace std;
using namespace jstreams;
using lucene::document::Document;
using lucene::document::Field;
using lucene::util::Reader;

CLuceneIndexWriter::CLuceneIndexWriter() {
    activewriter = 0;
}
CLuceneIndexWriter::~CLuceneIndexWriter() {
    // close all writers
    vector<lucene::index::IndexWriter*>::iterator i;
    for (i = writers.begin(); i != writers.end(); ++i) {
        lucene::index::IndexWriter* writer = *i;
	writer->optimize();
        writer->close();
	delete writer;
	//delete m_analyzer;
    }
}
void
CLuceneIndexWriter::addIndexWriter() {
    lucene::index::IndexWriter* writer = new lucene::index::IndexWriter("",0,0);
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

    if (writers.size() < activewriter) {
        addIndexWriter();
    }
    lucene::index::IndexWriter* writer = writers[activewriter];
    activewriter++;
    writer->addDocument(doc);
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
/*
    Close all left open indexwriters for this path.
*/
void
CLuceneIndexWriter::finishIndexable(const Indexable* idx) {
    Document *doc = docs[idx];
    if (doc) {
        if (writers.size() < activewriter) {
            addIndexWriter();
        }
        writers[activewriter]->addDocument(doc);
        delete doc;
        docs[idx] = 0;
    }
}

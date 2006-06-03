#include "cluceneindexwriter.h"
#include "cluceneindexmanager.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>
#include "stringreader.h"
#include "inputstreamreader.h"
#include <sstream>
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
CLuceneIndexWriter::addText(const Indexable* idx, const char* text,
        int32_t length) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->getWriterData());
    doc->content.append(text, length);
}
void
CLuceneIndexWriter::setField(const Indexable* idx, const string& fieldname,
        const string& value) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->getWriterData());
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
    setField(idx, "path", idx->getName());
//    printf("%s\n", idx->getName().c_str());
    ostringstream o;
    o << (int)idx->getDepth();
    setField(idx, "depth", o.str());
    o.str("");
    o << idx->getMTime();
    setField(idx, "mtime", o.str());
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->getWriterData());
    if (doc->content.length() > 0) {
#if defined(_UCS2)
        StringReader<char> sr(doc->content.c_str(), doc->content.length(),
            false);
        InputStreamReader streamreader(&sr);
        Reader* reader = new Reader(&streamreader, false);
        doc->doc.add( *Field::Text(L"content", reader, true) );
#else
        doc->doc.add(*Field::Text("content", doc->content.c_str(), true));
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

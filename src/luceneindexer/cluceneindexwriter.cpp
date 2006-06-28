#include "cluceneindexwriter.h"
#include "cluceneindexmanager.h"
#include <CLucene/clucene-config.h>
#include <CLucene.h>
#include <CLucene/search/QueryFilter.h>
#include "stringreader.h"
#include "inputstreamreader.h"
#include <sstream>
#include <assert.h>

using lucene::analysis::Analyzer;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::document::Document;
using lucene::document::Field;
using lucene::index::IndexWriter;
using lucene::index::Term;
using lucene::search::IndexSearcher;
using lucene::search::Hits;
using lucene::search::PrefixQuery;
using lucene::search::QueryFilter;
using lucene::util::BitSet;

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
    setField(idx, "encoding", idx->getEncoding());
    setField(idx, "mimetype", idx->getMimeType());
//    printf("%i %s\n", idx->getDepth(), idx->getName().c_str());
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
        const wchar_t* data;
        int32_t nread = streamreader.read(data, 10000000, 0);
        if (nread > 0) {
            wchar_t *naughty = (wchar_t*)(data);
            naughty[nread-1] = '\0';
            doc->doc.add(*Field::Text(L"content", naughty));
        }
#else
        doc->doc.add(*Field::Text("content", doc->content.c_str());
#endif
        lucene::index::IndexWriter* writer = manager->refWriter();
        try {
            writer->addDocument(&doc->doc);
        } catch (CLuceneError& err) {
            fprintf(stderr, "%s: %s\n", idx->getName().c_str(), err.what());
        }
    } else {
        lucene::index::IndexWriter* writer = manager->refWriter();
        try {
            writer->addDocument(&doc->doc);
        } catch (CLuceneError& err)  {
            fprintf(stderr, "%s: %s\n", idx->getName().c_str(), err.what());
        }
    }
    manager->derefWriter();
    delete doc;
}
void
CLuceneIndexWriter::deleteEntries(const std::vector<std::string>& entries) {
    for (uint i=0; i<entries.size(); ++i) {
        deleteEntry(entries[i]);
    }
}
void
CLuceneIndexWriter::deleteEntry(const string& entry) {
    lucene::index::IndexReader* reader = manager->refReader();

    TCHAR tstr[CL_MAX_DIR];
    STRCPY_AtoT(tstr, entry.c_str(), CL_MAX_DIR);
    Term* term = _CLNEW Term(_T("path"), tstr);
    PrefixQuery* query = _CLNEW PrefixQuery(term);
    QueryFilter* filter = _CLNEW QueryFilter(query);
    BitSet* bits;
    try {
        bits = filter->bits(reader);
    } catch (CLuceneError& err) {
        fprintf(stderr, "error creating filter %s: %s\n", entry.c_str(),
            err.what());
        bits = 0;
    }
    if (bits) {
        for (int32_t i = 0; i < bits->size(); ++i) {
            if (bits->get(i) && !reader->isDeleted(i)) {
                reader->deleteDocument(i);
            }
        }
    }
    _CLDELETE(filter);
    _CLDELETE(query);
    _CLDECDELETE(term);
    manager->derefReader();
}

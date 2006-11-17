/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "jstreamsconfig.h"
#include "cluceneindexwriter.h"
#include "cluceneindexmanager.h"
#include <CLucene.h>
#include <CLucene/search/QueryFilter.h>
#include "stringreader.h"
#include "inputstreamreader.h"
#include "indexable.h"
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
setField(const jstreams::Indexable* idx, const TCHAR* fn,
        const std::string& value) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->getWriterData());
#if defined(_UCS2)
    TCHAR fv[CL_MAX_DIR];
    STRCPY_AtoT(fv, value.c_str(), CL_MAX_DIR);
    doc->doc.add( *Field::Keyword(fn, fv) );
#else
    doc->doc.add( *Field::Keyword(fn, value.c_str()) );
#endif
}
void
CLuceneIndexWriter::setField(const Indexable* idx, const string& fieldname,
        const string& value) {
#if defined(_UCS2)
    TCHAR fn[CL_MAX_DIR];
    STRCPY_AtoT(fn, fieldname.c_str(), CL_MAX_DIR);
    ::setField(idx, fn, value);
#else
    ::setField(idx, fieldname.c_str(), value);
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
    ::setField(idx, _T("path"), idx->getPath());
    string field = idx->getEncoding();
    if (field.length()) ::setField(idx, _T("encoding"), field);
    field = idx->getMimeType();
    if (field.length()) ::setField(idx, _T("mimetype"), field);
    field = idx->getFileName();
    if (field.length()) ::setField(idx, _T("filename"), field);
    field = idx->getExtension();
    if (field.length()) ::setField(idx, _T("ext"), field);
    ostringstream o;
    o << (int)idx->getDepth();
    ::setField(idx, _T("depth"), o.str());
    o.str("");
    {
        char tmp[100];
        snprintf(tmp,100,"%llu",(uint64_t)idx->getMTime());
        o << tmp;
    }
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->getWriterData());
    ::setField(idx, _T("mtime"), o.str());
    StringReader<char>* sr = 0;
    InputStreamReader* streamreader = 0;
    lucene::util::Reader* reader = 0;
    if (doc->content.length() > 0) {
#if defined(_UCS2)
        sr = new StringReader<char>(doc->content.c_str(), doc->content.length(),
            false);
        streamreader = new InputStreamReader(sr);
        reader = new lucene::util::Reader(streamreader, false);
        doc->doc.add(*Field::Text(L"content", reader));
#else
        doc->doc.add(*Field::Text("content", doc->content.c_str()) );
#endif
    }
    lucene::index::IndexWriter* writer = manager->refWriter();
    if (writer) {
        try {
            writer->addDocument(&doc->doc);
        } catch (CLuceneError& err) {
            fprintf(stderr, "%s: %s\n", idx->getPath().c_str(), err.what());
        }
    }
    manager->derefWriter();
    delete doc;
    if (sr) {
        delete sr;
        delete streamreader;
    }
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

//    QueryBitset qbs = manager->getBitSets()->getBitset("path:"+entry);

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
        _CLDELETE(bits);
    }
    _CLDELETE(filter);
    _CLDELETE(query);
    _CLDECDELETE(term);
    manager->derefReader();
}
void
CLuceneIndexWriter::deleteAllEntries() {
    manager->deleteIndex();
}

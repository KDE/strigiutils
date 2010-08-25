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

#include "cluceneindexwriter.h"
#include "tcharutils.h"
#include <CLucene.h>
#include <CLucene/store/Lock.h>
#include "cluceneindexreader.h"
#include "cluceneindexmanager.h"
#include <CLucene/search/PrefixQuery.h>
#include <sstream>
#include <assert.h>
#include <iostream>

using lucene::document::Document;
using lucene::document::Field;
using lucene::index::IndexWriter;
using lucene::index::Term;
using lucene::index::TermDocs;
using lucene::search::BooleanQuery;
using lucene::search::IndexSearcher;
using lucene::search::Hits;
using lucene::search::PrefixFilter;
using lucene::search::TermQuery;
using lucene::search::Query;
using lucene::util::BitSet;

using lucene::util::Reader;
using namespace std;
using namespace Strigi;
typedef boost::shared_ptr<lucene::index::Term> LuceneTerm;

struct CLuceneDocData {
    lucene::document::Document doc;
    std::string content;
};

CLuceneIndexWriter::CLuceneIndexWriter(CLuceneIndexManager* m):
    manager(m), doccount(0) {
    string contentID(FieldRegister::contentFieldName.c_str());
    wstring cID(utf8toucs2(contentID));
    addMapping(_T(""),cID.c_str());
}
CLuceneIndexWriter::~CLuceneIndexWriter() {
}
const wchar_t*
CLuceneIndexWriter::systemlocation() {
    const static wstring s(utf8toucs2(FieldRegister::pathFieldName));
    return s.c_str();
}
namespace {
const wchar_t*
parentlocation() {
    const static wstring s(utf8toucs2(FieldRegister::parentLocationFieldName));
    return s.c_str();
}
}
void
CLuceneIndexWriter::addText(const AnalysisResult* idx, const char* text,
        int32_t length) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->writerData());
    doc->content.append(text, length);
}

typedef map<wstring, wstring> CLuceneIndexWriterFieldMapType;
CLuceneIndexWriterFieldMapType CLuceneIndexWriterFieldMap;

void CLuceneIndexWriter::addMapping(const TCHAR* from, const TCHAR* to){
    CLuceneIndexWriterFieldMap[from] = to;
}
const TCHAR*
CLuceneIndexWriter::mapId(const TCHAR* id) {
    if (id == 0) id = _T("");
    CLuceneIndexWriterFieldMapType::iterator itr
        = CLuceneIndexWriterFieldMap.find(id);
    if (itr == CLuceneIndexWriterFieldMap.end()) {
        return id;
    } else {
        return itr->second.c_str();
    }
}
void
CLuceneIndexWriter::addValue(const AnalysisResult* idx,
        AnalyzerConfiguration::FieldType type, const TCHAR* name,
        const TCHAR* value) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->writerData());
    int config = 0;
    if ((type & AnalyzerConfiguration::Stored) == AnalyzerConfiguration::Stored){
        config |= Field::STORE_YES;
    } else {
        config |= Field::STORE_NO;
    }

    if ((type & AnalyzerConfiguration::Indexed)
            == AnalyzerConfiguration::Indexed) {
        if ((type & AnalyzerConfiguration::Tokenized)
                == AnalyzerConfiguration::Tokenized) {
            config |= Field::INDEX_TOKENIZED;
        } else {
            config |= Field::INDEX_UNTOKENIZED;
        }
    } else {
        config |= Field::INDEX_NO;
    }

    Field* field = new Field(name, value, config);
    doc->doc.add(*field);
}
void
CLuceneIndexWriter::addValue(const AnalysisResult* idx,
        AnalyzerConfiguration::FieldType type, const TCHAR* fn,
        const std::string& value) {
    addValue(idx, type, CLuceneIndexWriter::mapId(fn), utf8toucs2(value).c_str());
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, const std::string& value) {
    AnalyzerConfiguration::FieldType type
        = idx->config().indexType(field);
    if (type == AnalyzerConfiguration::None) return;
    addValue(idx, type, utf8toucs2(field->key()).c_str(), value);
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, uint32_t value) {
    ostringstream o;
    o << value;
    addValue(idx, field, o.str());
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, int32_t value) {
    ostringstream o;
    o << value;
    addValue(idx, field, o.str());
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field,
        const unsigned char* data, uint32_t size) {
    addValue(idx, field, string((const char*)data, (string::size_type)size));
}
void
CLuceneIndexWriter::addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, double value) {
    ostringstream o;
    o << value;
    addValue(idx, field, o.str());
}
void
CLuceneIndexWriter::startAnalysis(const AnalysisResult* idx) {
    doccount++;
    CLuceneDocData*doc = new CLuceneDocData();
    idx->setWriterData(doc);
}
/*
    Close all left open indexwriters for this path.
*/
void
CLuceneIndexWriter::finishAnalysis(const AnalysisResult* idx) {
    CLuceneDocData* doc = static_cast<CLuceneDocData*>(idx->writerData());
    wstring c(utf8toucs2(doc->content));

    if (doc->content.length() > 0) {
      const TCHAR* mappedFn = mapId(_T(""));

      // add the stored field as compressed and indexed
      doc->doc.add(*new Field(mappedFn, c.c_str(), Field::STORE_YES | Field::STORE_COMPRESS | Field::INDEX_TOKENIZED));
    }
    lucene::index::IndexWriter* writer = manager->refWriter();
    if (writer) {
        try {
            writer->addDocument(&doc->doc);
            fprintf(stderr, "added %s\n", idx->path().c_str());
        } catch (CLuceneError& err) {
            fprintf(stderr, "%s: %s\n", idx->path().c_str(), err.what());
        }
    }
    manager->derefWriter();
    delete doc;
}
void
CLuceneIndexWriter::deleteEntries(const std::vector<std::string>& entries) {
    // make sure the index reader is up to date
    lucene::index::IndexReader* reader = manager->checkReader(true);
    if (reader == NULL) {
        fprintf(stderr,"cannot delete entry: lucene reader cannot be opened\n");
        return;
    }

    lucene::index::IndexWriter* writer = manager->refWriter();

    for (uint i=0; i<entries.size(); ++i) {
        deleteEntry(entries[i], writer, reader);
    }
    writer->flush();
    reader->commit();

    manager->derefWriter();
}
void
CLuceneIndexWriter::deleteEntry(const string& entry, lucene::index::IndexWriter* writer, lucene::index::IndexReader* reader) {
    wstring path(utf8toucs2(entry));
{
    LuceneTerm t(_CLNEW Term(systemlocation(), path.c_str()));
    writer->deleteDocuments(t);
}
{
    LuceneTerm t(_CLNEW Term(parentlocation(), path.c_str()));
    writer->deleteDocuments(t);

}
{
    // delete all deeper nested files
    wstring v = utf8toucs2(entry+"/");
    LuceneTerm t(_CLNEW Term(parentlocation(), v.c_str()));
    PrefixFilter* filter = _CLNEW PrefixFilter(t);
    BitSet* b = filter->bits(reader);
    _CLDELETE(filter);
    int32_t size = b->size();
    for (int id = 0; id < size; ++id) {
        if (b->get(id) && !reader->isDeleted(id)) {
            reader->deleteDocument(id);
        }
    }
    _CLDELETE(b);
}
}
void
CLuceneIndexWriter::deleteAllEntries() {
    lucene::index::IndexReader* reader = manager->checkReader();
    if ( reader != NULL ){
      for ( int32_t i=0;i<reader->maxDoc();i++ ){
        reader->deleteDocument(i);
      }
      reader->flush();
    }
}
void
CLuceneIndexWriter::commit() {
    lucene::index::IndexWriter* writer = manager->refWriter();
    writer->flush();
    manager->derefWriter();
}

void
CLuceneIndexWriter::initWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end = f.fields().end();
    for (i = f.fields().begin(); i != end; ++i) {
        i->second->setWriterData(0);
    }
}
void
CLuceneIndexWriter::releaseWriterData(const FieldRegister& f) {
    map<string, RegisteredField*>::const_iterator i;
    map<string, RegisteredField*>::const_iterator end = f.fields().end();
    for (i = f.fields().begin(); i != end; ++i) {
        delete static_cast<int*>(i->second->writerData());
    }
}

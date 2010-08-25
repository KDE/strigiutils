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

#include "cluceneindexreader.h"
#include <strigi/strigiconfig.h>
#include <strigi/query.h>
#include <strigi/queryparser.h>
#include <strigi/variant.h>
#include <strigi/textutils.h>
#include "cluceneindexmanager.h"
#include "timeofday.h"
#include "tcharutils.h"
#include <CLucene/search/QueryFilter.h>
#include <CLucene/index/Terms.h>
#include <CLucene/document/FieldSelector.h>
#include <CLucene/store/RAMDirectory.h>
#include <strigi/fieldtypes.h>
#include <sstream>
#include <iostream>
#include <cassert>
#include <climits>

using lucene::search::Hits;
using lucene::search::IndexSearcher;
using lucene::document::Document;
using lucene::document::Field;
using lucene::index::Term;
using lucene::index::TermDocs;
using lucene::index::TermEnum;
using lucene::search::TermQuery;
using lucene::search::WildcardQuery;
using lucene::search::BooleanQuery;
using lucene::search::Query;
using lucene::search::RangeQuery;
using lucene::search::QueryFilter;
using lucene::search::HitCollector;
using lucene::util::BitSet;
using lucene::document::DocumentFieldEnumeration;
using Strigi::IndexedDocument;
using Strigi::Variant;
using Strigi::FieldRegister;
typedef boost::shared_ptr<lucene::index::Term> LuceneTerm;

using namespace std;

class HitCounter : public HitCollector {
private:
    int32_t m_count;
    void collect (const int32_t doc, const float_t score) { m_count++; }
public:
    HitCounter() :m_count(0) {}
    int32_t count() const { return m_count; }
};

class StringMapFieldSelector : public lucene::document::FieldSelector {
  std::vector<std::wstring> fullFields;
public:
  StringMapFieldSelector(const std::vector<std::string>& fullFields){
    std::wstring str;
    for ( std::vector<std::string>::const_iterator itr = fullFields.begin();
          itr != fullFields.end(); itr ++ ){
      this->fullFields.push_back(utf8toucs2(*itr));
    }
  }

  FieldSelectorResult accept(const TCHAR* field) const{
    for ( std::vector<std::wstring>::const_iterator itr = fullFields.begin();
          itr != fullFields.end(); itr ++ ){
      if ( (*itr).compare(field) == 0 )
        return lucene::document::FieldSelector::LOAD;
    }
    return lucene::document::FieldSelector::NO_LOAD;
  }
};

class CLuceneIndexReader::Private {
public:
    static const wchar_t* systemlocation();
    static const wchar_t* mtime();
    static const wchar_t* mimetype();
    static const wchar_t* size();
    static const wchar_t* content();
    static const wchar_t* parentlocation();
    CLuceneIndexReader& reader;
    Private(CLuceneIndexReader& r) :reader(r) {}

    static LuceneTerm createTerm(const wchar_t* name, const string& value);
    static LuceneTerm createKeywordTerm(const wchar_t* name, const string& value);
    static LuceneTerm createWildCardTerm(const wchar_t* name, const string& value);
    Query* createQuery(const Strigi::Query& query);
    Query* createSimpleQuery(const Strigi::Query& query);
    static Query* createSingleFieldQuery(const string& field,
        const Strigi::Query& query);
    Query* createNoFieldQuery(const Strigi::Query& query);
    Query* createMultiFieldQuery(const Strigi::Query& query);
    BooleanQuery* createBooleanQuery(const Strigi::Query& query);
    static void addField(lucene::document::Field* field, IndexedDocument&);
    Variant getFieldValue(lucene::document::Field* field, Variant::Type) const;

    vector<IndexedDocument> strigiSpecial(const string& command);
};
const wchar_t*
CLuceneIndexReader::Private::systemlocation() {
    const static wstring s(utf8toucs2(FieldRegister::pathFieldName));
    return s.c_str();
}
const wchar_t*
CLuceneIndexReader::Private::mtime() {
    const static wstring s(utf8toucs2(FieldRegister::mtimeFieldName));
    return s.c_str();
}
const wchar_t*
CLuceneIndexReader::Private::mimetype() {
    const static wstring s(utf8toucs2(FieldRegister::mimetypeFieldName));
    return s.c_str();
}
const wchar_t*
CLuceneIndexReader::Private::size() {
    const static wstring s(utf8toucs2(FieldRegister::sizeFieldName));
    return s.c_str();
}
const wchar_t*
CLuceneIndexReader::Private::parentlocation() {
    const static wstring s(utf8toucs2(FieldRegister::parentLocationFieldName));
    return s.c_str();
}
const wchar_t*
CLuceneIndexReader::Private::content() {
    const static wstring s(utf8toucs2(FieldRegister::contentFieldName));
    return s.c_str();
}

CLuceneIndexReader::CLuceneIndexReader(CLuceneIndexManager* m,
    const string& dir) :manager(m), p(new Private(*this)), dbdir(dir){
}

CLuceneIndexReader::~CLuceneIndexReader() {
    delete p;
}

typedef map<wstring, wstring> CLuceneIndexReaderFieldMapType;
CLuceneIndexReaderFieldMapType CLuceneIndexReaderFieldMap;

void CLuceneIndexReader::addMapping(const TCHAR* from, const TCHAR* to){
    CLuceneIndexReaderFieldMap[from] = to;
}
const TCHAR*
CLuceneIndexReader::mapId(const TCHAR* id) {
    if (CLuceneIndexReaderFieldMap.size() == 0) {
        string contentID(FieldRegister::contentFieldName.c_str());
        wstring cID(utf8toucs2(contentID));
        addMapping(_T(""), cID.c_str());
    }
    if (id == 0) {
        id = _T("");
    }
    CLuceneIndexReaderFieldMapType::iterator itr
        = CLuceneIndexReaderFieldMap.find(id);
    if (itr == CLuceneIndexReaderFieldMap.end()) {
        return id;
    } else {
        return itr->second.c_str();
    }
}
#ifdef _UCS2
wstring
CLuceneIndexReader::mapId(const char* id) {
    wstring tid = utf8toucs2(id);
    return mapId(tid.c_str());
}
#endif

LuceneTerm
CLuceneIndexReader::Private::createWildCardTerm(const wchar_t* name,
        const string& value) {
    return LuceneTerm(_CLNEW Term(name, utf8toucs2(value).c_str()));
}
LuceneTerm
CLuceneIndexReader::Private::createTerm(const wchar_t* name,
        const string& value) {
    wstring v = utf8toucs2(value);
    lucene::util::StringReader sr(v.c_str());
    lucene::analysis::standard::StandardAnalyzer a;
    lucene::analysis::TokenStream* ts = a.tokenStream(name, &sr);
    lucene::analysis::Token to;
    const wchar_t *tv;
    if ( ts->next(&to) != NULL ) {
        tv = to.termBuffer();
    } else {
        tv = v.c_str();
    }
    LuceneTerm t(_CLNEW Term(name, tv));
    _CLDELETE(ts);
    return t;
}
LuceneTerm
CLuceneIndexReader::Private::createKeywordTerm(const wchar_t* name,
        const string& value) {
    wstring v = utf8toucs2(value);
    LuceneTerm t(_CLNEW Term(name, v.c_str()));
    return t;
}
BooleanQuery*
CLuceneIndexReader::Private::createBooleanQuery(const Strigi::Query& query) {
    BooleanQuery* bq = _CLNEW BooleanQuery();
    bool isAnd = query.type() == Strigi::Query::And;
    const vector<Strigi::Query>& sub = query.subQueries();
    for (vector<Strigi::Query>::const_iterator i = sub.begin(); i != sub.end();
            ++i) {
        Query* q = createQuery(*i);
        bq->add(q, true, isAnd, i->negate());
    }
    return bq;
}
Query*
CLuceneIndexReader::Private::createQuery(const Strigi::Query& query) {
    return query.subQueries().size()
        ? createBooleanQuery(query)
        : createSimpleQuery(query);
}
Query*
CLuceneIndexReader::Private::createSimpleQuery(const Strigi::Query& query) {
    switch (query.fields().size()) {
    case 0:  return createSingleFieldQuery(FieldRegister::contentFieldName,
        query);//return createNoFieldQuery(query);
    case 1:  return createSingleFieldQuery(query.fields()[0], query);
    default: return createMultiFieldQuery(query);
    }
}
Query*
CLuceneIndexReader::Private::createSingleFieldQuery(const string& field,
        const Strigi::Query& query) {
    wstring fieldname = mapId(field.c_str());
    Query* q;
    LuceneTerm t;
    LuceneTerm nullTerm;
    const string& val = query.term().string();
    switch (query.type()) {
    case Strigi::Query::LessThan:
          t = createTerm(fieldname.c_str(), val.c_str());
          q = _CLNEW RangeQuery(nullTerm, t, false);
          break;
    case Strigi::Query::LessThanEquals:
          t = createTerm(fieldname.c_str(), query.term().string());
          q = _CLNEW RangeQuery(nullTerm, t, true);
          break;
    case Strigi::Query::GreaterThan:
          t = createTerm(fieldname.c_str(), query.term().string());
          q = _CLNEW RangeQuery(t, nullTerm, false);
          break;
    case Strigi::Query::GreaterThanEquals:
          t = createTerm(fieldname.c_str(), query.term().string());
          q = _CLNEW RangeQuery(t, nullTerm, true);
          break;
    case Strigi::Query::Keyword:
          t = createKeywordTerm(fieldname.c_str(), query.term().string());
          q = _CLNEW TermQuery(t);
          break;
    default:
          if (strpbrk(val.c_str(), "*?")) {
               t = createWildCardTerm(fieldname.c_str(), val);
               q = _CLNEW WildcardQuery(t);
          } else {
               t = createTerm(fieldname.c_str(), val);
               q = _CLNEW TermQuery(t);
          }
    }
    return q;
}
Query*
CLuceneIndexReader::Private::createMultiFieldQuery(const Strigi::Query& query) {
    BooleanQuery* bq = _CLNEW BooleanQuery();
    for (vector<string>::const_iterator i = query.fields().begin();
            i != query.fields().end(); ++i) {
        Query* q = createSingleFieldQuery(*i, query);
        bq->add(q, true, false, false);
    }
    return bq;
}
Query*
CLuceneIndexReader::Private::createNoFieldQuery(const Strigi::Query& query) {
    vector<string> fields = reader.fieldNames();
    BooleanQuery* bq = _CLNEW BooleanQuery();
    for (vector<string>::const_iterator i = fields.begin(); i != fields.end();
            ++i) {
        Query* q = createSingleFieldQuery(*i, query);
        bq->add(q, true, false, false);
    }
    return bq;
}
void
CLuceneIndexReader::Private::addField(lucene::document::Field* field,
        IndexedDocument& doc) {
    if (field->stringValue() == 0) return;
    string value(wchartoutf8(field->stringValue()));
    const TCHAR* name = field->name();
    if (wcscmp(name, content()) == 0) {
        doc.fragment = value;
    } else if (wcscmp(name, systemlocation()) == 0) {
        doc.uri = value;
    } else if (wcscmp(name, mimetype()) == 0) {
        doc.mimetype = value;
    } else if (wcscmp(name, mtime()) == 0) {
        doc.mtime=atol(value.c_str());
    } else if (wcscmp(name, size()) == 0) {
        string size = value;
        doc.size = atoi(size.c_str());
    } else {
        doc.properties.insert(make_pair<const string, string>(
            wchartoutf8(name), value));
    }
}
Variant
CLuceneIndexReader::Private::getFieldValue(lucene::document::Field* field,
        Variant::Type type) const {
    if (field->stringValue() == 0) return Variant();
    Variant v(wchartoutf8(field->stringValue()));
    if (type == Variant::b_val) {
         v = v.b();
    } else if (type == Variant::i_val) {
         v = v.i();
    } else if (type == Variant::as_val) {
         v = v.as();
    }
    return v;
}
int32_t
CLuceneIndexReader::countHits(const Strigi::Query& q) {
    lucene::index::IndexReader* reader = manager->checkReader();

    // if the query is empty, we return the number of files in the index
    if (q.term().string().size() == 0 && q.subQueries().size() == 0) {
        return countDocuments();
    }

    Query* bq = p->createQuery(q);
    if (reader == 0) {
        return 0;
    }
    IndexSearcher searcher(reader);
    vector<IndexedDocument> results;
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(bq);
        s = hits->length();
    } catch (CLuceneError& err) {
/*        HitCounter counter;
        QueryFilter* filter = _CLNEW QueryFilter(&bq);
        try {
        BitSet* bits = filter->bits(reader);
        int32_t n = bits->size();
        for (int32_t i=0; i<n; ++i) {
            if (bits->get(i)) s++;
        }
        } catch (CLuceneError& err2) {
            printf("ccould not query: %s\n", err.what());
        }
        try {
            searcher._search(0, filter, &counter);
        } catch (CLuceneError& err2) {
            printf("ccould not query: %s\n", err.what());
        }
        s = counter.count();

        printf("counted %i hits\n", count);
        // try to do a constant score query
        //QueryFilter* filter = _CLNEW QueryFilter(&bq);
        ConstantScoreQuery csq(filter);*/
        fprintf(stderr, "could not query: %s\n", err.what());
    }
    delete hits;
    searcher.close();
    _CLDELETE(bq);
    return s;
}
vector<IndexedDocument>
CLuceneIndexReader::query(const Strigi::Query& q, int off, int max) {
    vector<IndexedDocument> results;
    lucene::index::IndexReader* reader = manager->checkReader();
    if (!reader) {
        return results;
    }

    // handle special commands
    if (q.fields().size() == 1 && q.fields()[0].empty()
            && q.term().string().substr(0, 14) == "strigispecial:") {
        return p->strigiSpecial(q.term().string());
    }

    Query* bq = p->createQuery(q);
    IndexSearcher searcher(reader);
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(bq);
        s = hits->length();
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not query: %s\n", err.what());
    }
    if (off < 0) off = 0;
    max += off;
    if (max < 0) max = s;
    if (max > s) max = s;
    if (max > off) {
        results.reserve(max-off);
    }
    for (int i = off; i < max; ++i) {
        Document *d = &hits->doc(i);
        IndexedDocument doc;
        doc.score = hits->score(i);
        const Document::FieldsType& fields = *d->getFields();
        printf("hit doc id %d with %d fields\n", (int)hits->id(i), (int)fields.size());
        Document::FieldsType::const_iterator itr = fields.begin();
        if ( itr == fields.end() )
        	printf("E");
        if ( itr == fields.begin() )
        	printf("B");

        while (itr != fields.end()){
        	printf("XXXX");
            Field* f = *itr;
            Private::addField(f, doc);
            itr++;
        }
        printf("done\n");
        results.push_back(doc);
    }
    if (hits) {
        _CLDELETE(hits);
    }
    searcher.close();
    _CLDELETE(bq);
    return results;
}
void
CLuceneIndexReader::getDocuments(const std::vector<std::string>& fullFields,
        const std::vector<Strigi::Variant::Type>& types,
        std::vector<std::vector<Strigi::Variant> >& result, int off, int max) {
    lucene::index::IndexReader* reader = manager->checkReader();

    int pos = 0;
    int maxDoc = reader->maxDoc();
    for (int i=0; i<off; i++) {
        while (pos < maxDoc && reader->isDeleted(pos)) pos++;
        if (pos == maxDoc) return;
        pos++;
    }
    if (max < 0) max = 0;
    result.resize(max);
    StringMapFieldSelector fieldSelector(fullFields);
    Document d;
    for (int i = 0; i < max && pos < maxDoc; ++i) {
        while (pos < maxDoc && reader->isDeleted(pos)) pos++;
        if (pos == maxDoc || !reader->document(pos++, d, &fieldSelector)) {
            continue;
        }

        vector<Variant>& doc = result[i];
        doc.clear();
        doc.resize(fullFields.size());

        const Document::FieldsType& fields = *d.getFields();
        for ( Document::FieldsType::const_iterator itr = fields.begin();
              itr != fields.end(); itr++ ) {
            Field* field = *itr;
            string name(wchartoutf8(field->name()));
            for (uint j = 0; j < fullFields.size(); ++j) {
                if (fullFields[j] == name) {
                    doc[j] = p->getFieldValue(field, types[j]);
                }
            }
        }
    }
}
void
CLuceneIndexReader::getHits(const Strigi::Query& q,
        const std::vector<std::string>& fields,
        const std::vector<Strigi::Variant::Type>& types,
        std::vector<std::vector<Strigi::Variant> >& result, int off, int max) {
    result.clear();
    lucene::index::IndexReader* reader = manager->checkReader();
    if (reader==NULL || types.size() < fields.size()) {
        return;
    }

    vector<string> fullFields;
    fullFields.resize(fields.size());
    for (size_t i = 0; i < fields.size(); i++) {
        if (fields[i].compare(0, 6, "xesam:") == 0) {
            fullFields[i].assign(
                "http://freedesktop.org/standards/xesam/1.0/core#"
                + fields[i].substr(6));
        } else if (fields[i].compare(0, 4, "nie:") == 0) {
            fullFields[i].assign(
                "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#"
                + fields[i].substr(4));
        } else {
            fullFields[i].assign(fields[i]);
        }
    }

    // if the query is empty, we return the number of files in the index
    if (q.term().string().size() == 0 && q.subQueries().size() == 0) {
        getDocuments(fullFields, types, result, off, max);
        return;
    }

    Query* bq = p->createQuery(q);
    IndexSearcher searcher(reader);
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(bq);
        s = hits->length();
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not query: %s\n", err.what());
    }
    if (off < 0) off = 0;
    max += off;
    if (max < 0) max = s;
    if (max > s) max = s;
    if (max > off) {
        result.reserve(max-off);
    }
    result.resize(max-off);
    for (int i = off; i < max; ++i) {
        Document *d = &hits->doc(i);
        vector<Variant>& doc = result[i-off];
        doc.clear();
        doc.resize(fields.size());

        const Document::FieldsType fields = *d->getFields();
        for ( Document::FieldsType::const_iterator itr = fields.begin();
              itr != fields.end(); itr++ ) {
            Field* field = *itr;

            string name(wchartoutf8(field->name()));
            for (uint j = 0; j < fullFields.size(); ++j) {
                if (fullFields[j] == name) {
                    doc[j] = p->getFieldValue(field, types[j]);
                }
            }
        }
    }
    if (hits) {
        _CLDELETE(hits);
    }
    searcher.close();
    _CLDELETE(bq);
}
int32_t
CLuceneIndexReader::countDocuments() {
    return manager->countDocuments();
}
int32_t
CLuceneIndexReader::countWords() {
    return manager->countWords();
}
int64_t
CLuceneIndexReader::indexSize() {
    return manager->indexSize();
}
int64_t
CLuceneIndexReader::documentId(const string& uri) {
    lucene::index::IndexReader* reader = manager->checkReader();
    if (reader == NULL) return -1;
    int64_t id = -1;

    LuceneTerm term(_CLNEW Term(mapId(Private::systemlocation()), utf8toucs2( uri ).c_str()));
    TermDocs* docs = reader->termDocs(term);
    if (docs->next()) {
        id = docs->doc();
    }
    _CLDELETE(docs);

    if (id != -1 && reader->isDeleted((int32_t)id)) {
        id = -1;
    }

    return id;
}
/**
 * Retrieve the mtime of the document with id @docid. If this document
 * is not in the index, the time 0 is returned.
 **/
time_t
CLuceneIndexReader::mTime(int64_t docid) {
    if (docid < 0) return 0;
    lucene::index::IndexReader* reader = manager->checkReader(true);
    if (reader == NULL) return 0;
    time_t mtime = 0;
    Document d;
    lucene::document::MapFieldSelector fieldSelector;
    fieldSelector.add(Private::mtime());
    if ( reader->document((int32_t)docid, d, &fieldSelector) ) {
        const TCHAR* v = d.get(Private::mtime());
        mtime = atoi(wchartoutf8( v ).c_str());
    }
    return mtime;
}
/**
 * Retrieve the mtime of the document with id @docid. If this document
 * is not in the index, the time 0 is returned.
 **/
time_t
CLuceneIndexReader::mTime(const std::string& uri) {
    return mTime(documentId(uri));
}
class Histogram {
public:
    vector<pair<string,uint32_t> > h;
    vector<int> values;
};
#include <time.h>
vector<pair<string,uint32_t> >
makeTimeHistogram(const vector<int>& v) {
    map<int32_t, int32_t> m;
    vector<int32_t>::const_iterator i;
    struct tm t;
    for (i = v.begin(); i < v.end(); ++i) {
        time_t ti = *i;
#ifdef _WIN32
        t = *localtime( &ti );   // is thread-safe on win32
#else
        localtime_r(&ti, &t);
#endif
        int32_t c = 10000*t.tm_year + 100*t.tm_mon + t.tm_mday;
        m[c]++;
    }
    vector<pair<string,uint32_t> > h;
    h.reserve(m.size());
    ostringstream str;
    map<int32_t,int32_t>::const_iterator j;
    for (j = m.begin(); j != m.end(); ++j) {
        str << j->first + 19000100;
        h.push_back(make_pair<string,uint32_t>(str.str(), j->second));
        str.str("");
    }
    return h;
}
vector<pair<string,uint32_t> >
makeHistogram(const vector<int>& v, int min, int max) {
    map<int32_t, int32_t> m;
    vector<int32_t>::const_iterator i;
    for (i = v.begin(); i < v.end(); ++i) {
        m[*i]++;
    }
    vector<pair<string,uint32_t> > h;
    h.reserve(m.size());
    ostringstream str;
    map<int32_t,int32_t>::const_iterator j;
    for (j = m.begin(); j != m.end(); ++j) {
        str << j->first;
        h.push_back(make_pair<string,uint32_t>(str.str(), j->second));
        str.str("");
    }
    return h;
}
vector<pair<string,uint32_t> >
CLuceneIndexReader::histogram(const string& query,
        const string& fieldname, const string& labeltype) {
    vector<pair<string,uint32_t> > h;
    lucene::index::IndexReader* reader = manager->checkReader();
    if (reader == NULL) {
        return h;
    }
    Strigi::QueryParser parser;
    Strigi::Query q = parser.buildQuery(query);
    Query* bq = p->createQuery(q);
    IndexSearcher searcher(reader);
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(bq);
        s = hits->length();
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not query: %s\n", err.what());
    }
    wstring field = utf8toucs2(fieldname);
    int32_t max = INT_MIN;
    int32_t min = INT_MAX;
    vector<int32_t> values;
    values.reserve(s);
    char* end;
    for (int i = 0; i < s; ++i) {
        Document *d = &hits->doc(i);
        const TCHAR* v = d->get(field.c_str());
        if (v) {
            int val = (int)strtol(wchartoutf8( v ).c_str(), &end, 10);
            if ( *end != 0) {
                _CLDELETE(hits);
                return h;
            }
            values.push_back(val);
            max = (max>val) ?max :val;
            min = (min<val) ?min :val;
        }
    }
    if (hits) {
        _CLDELETE(hits);
    }
    searcher.close();
    _CLDELETE(bq);
    if (fieldname == FieldRegister::mtimeFieldName || labeltype == "time") {
        return makeTimeHistogram(values);
    } else {
        return makeHistogram(values, min, max);
    }
}
vector<string>
CLuceneIndexReader::fieldNames() {
    vector<string> s;
    lucene::index::IndexReader* reader = manager->checkReader();
    if (reader == NULL) {
        return s;
    }
    StringArrayWithDeletor names;
    reader->getFieldNames(lucene::index::IndexReader::ALL, names);
    for(StringArrayWithDeletor::iterator n = names.begin();
        n != names.end();
        n++ ){
        string str(wchartoutf8(*n));
        s.push_back(str);
        _CLDELETE_ARRAY(*n);
        n++;
    }
    return s;
}
int32_t
CLuceneIndexReader::countKeywords(const string& keywordprefix,
        const vector<string>& fieldnames) {
    return 2;
}
vector<string>
CLuceneIndexReader::keywords(const string& keywordmatch,
        const vector<string>& fieldnames, uint32_t max, uint32_t offset) {

    vector<string> k;
    lucene::index::IndexReader* reader = manager->checkReader();
    if ( reader == NULL ){
      return k;
    }

    vector<string> fn;
    if (fieldnames.size()) {
        fn = fieldnames;
    } else {
        fn = fieldNames();
    }
    set<wstring> s;
    wstring prefix = utf8toucs2(keywordmatch);
    const wchar_t* prefixtext = prefix.c_str();
    string::size_type prefixLen = prefix.length();
    vector<string>::const_iterator i;
    LuceneTerm lastTerm;
    for (i = fn.begin(); i != fn.end() && s.size() << max; ++i) {
         wstring fieldname(utf8toucs2(*i));
         LuceneTerm term(_CLNEW Term(fieldname.c_str(), prefix.c_str()));
         TermEnum* enumerator = reader->terms(term);
         do {
             lastTerm = enumerator->term();
             if (lastTerm) {
                 if (prefixLen > lastTerm->textLength()
                         || wcsncmp(lastTerm->text(), prefixtext, prefixLen)
                             != 0) {
                     break;
                 }
                 s.insert(lastTerm->text());
             }
         } while (enumerator->next() && s.size() < max);
    }

    k.reserve(s.size());
    set<wstring>::const_iterator j;
    for (j = s.begin(); j != s.end(); ++j) {
        k.push_back(wchartoutf8(*j));
    }
    return k;
}
void
CLuceneIndexReader::getChildren(const std::string& parent,
            std::map<std::string, time_t>& children) {
    children.clear();
    // force a fresh reader. This is important because the function
    // getChildren is essential for updating the index
    lucene::index::IndexReader* reader = manager->checkReader(true);
    if ( reader == NULL ) {
        return;
    }
    // build a query
    Query* q = _CLNEW TermQuery(
        Private::createKeywordTerm(Private::parentlocation(), parent));
    IndexSearcher searcher(reader);
    Hits* hits = 0;
    int nhits = 0;
    try {
        hits = searcher.search(q);
        nhits = hits->length();
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not query: %s\n", err.what());
    }
    const TCHAR* mtime = mapId(Private::mtime());
    for (int i = 0; i < nhits; ++i) {
        Document* d = &hits->doc(i);

        const TCHAR* v = d->get(mtime);
        // check that mtime is defined for this document
        if (v) {
            time_t mtime = atoi(wchartoutf8( v ).c_str());
            v = d->get(Private::systemlocation());
            if (v) {
                children[wchartoutf8( v )] = mtime;
            }
        }

    }
    if (hits) {
        _CLDELETE(hits);
    }
    searcher.close();
    _CLDELETE(q);
}
vector<IndexedDocument>
CLuceneIndexReader::Private::strigiSpecial(const string& command) {
    vector<IndexedDocument> r;
    lucene::index::IndexReader* reader = this->reader.manager->checkReader();
    if ( reader == NULL ){
      return r;
    }

cerr << "strigispecial " << command << endl;
    // we are going to count the size of each of the fields in this index
    // this requires that we loop through all fields
    lucene::index::TermEnum* terms = reader->terms();

    map<const TCHAR *, int64_t> lengths;
    while (terms->next()) {
        lengths[terms->term()->field()] += terms->term()->textLength();
       // cerr << wchartoutf8(terms->term()->field()) << '\t'
        //    << wchartoutf8(terms->term()->text()) << endl;
    }
    int64_t total = 0;
    for (map<const TCHAR *, int64_t>::const_iterator i = lengths.begin();
            i != lengths.end(); ++i) {
        cerr << wchartoutf8(i->first) << '\t' << i->second << endl;
        total += i->second;
    }
    terms->close();
    cerr << "total" << '\t' << total << endl;

    int32_t max = reader->numDocs();
    lucene::document::Document d;
    for (int32_t i=0; i < max; ++i) {
        if ( reader->document(i, d) ){ //TODO: use FieldSelectorResult::SIZE
          const Document::FieldsType& fields = *d.getFields();
          for ( Document::FieldsType::const_iterator itr = fields.begin();
                itr != fields.end(); itr++ ) {
              Field* f = *itr;

              if (f->isStored()) {
                  total += wcslen(f->stringValue());
              }
          }
        }
    }
    cerr << "total" << '\t' << total << endl;
    return r;
}

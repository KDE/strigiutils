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

using namespace std;

class HitCounter : public HitCollector {
private:
    int32_t m_count;
    void collect (const int32_t doc, const float_t score) { m_count++; }
public:
    HitCounter() :m_count(0) {}
    int32_t count() const { return m_count; }
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

    static Term* createTerm(const wchar_t* name, const string& value);
    static Term* createKeywordTerm(const wchar_t* name, const string& value);
    static Term* createWildCardTerm(const wchar_t* name, const string& value);
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
    const string& dir) :manager(m), p(new Private(*this)), dbdir(dir),
        reader(0) {
    otime.tv_sec = 0;
    otime.tv_usec = 0;
    openReader();
}

CLuceneIndexReader::~CLuceneIndexReader() {
    closeReader();
    delete p;
}
void
CLuceneIndexReader::openReader() {
    closeReader();
    doccount = -1;
    wordcount = -1;
    try {
        reader = lucene::index::IndexReader::open(manager->directory);
        // fprintf(stderr,
        // "READER at %s: %i\n", dbdir.c_str(), reader->numDocs());
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not create reader %s: %s\n", dbdir.c_str(),
            err.what());
        reader = 0;
    }
}
void
CLuceneIndexReader::closeReader() {
    if (reader == 0) return;
    try {
        reader->close();
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not close clucene: %s\n", err.what());
    }
    delete reader;
    reader = 0;
}
bool
CLuceneIndexReader::checkReader(bool enforceCurrent) {
    struct timeval mtime = manager->indexMTime();
    if (mtime.tv_sec != otime.tv_sec || mtime.tv_usec != otime.tv_usec) {
        if (enforceCurrent) {
            otime = mtime;
            closeReader();
        } else {
            struct timeval now;
            gettimeofday(&now, 0);
            if (now.tv_sec - otime.tv_sec > 60) {
                otime = mtime;
                closeReader();
            }
        }
    }
    if (reader == 0) {
        openReader();
    }
    return reader != NULL;
}

#ifdef _UCS2
typedef map<wstring, wstring> CLuceneIndexReaderFieldMapType;
#else
typedef map<string, string> CLuceneIndexReaderFieldMapType;
#endif
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

Term*
CLuceneIndexReader::Private::createWildCardTerm(const wchar_t* name,
        const string& value) {
    wstring v = utf8toucs2(value);
    return _CLNEW Term(name, v.c_str());
}
Term*
CLuceneIndexReader::Private::createTerm(const wchar_t* name,
        const string& value) {
    wstring v = utf8toucs2(value);
    lucene::util::StringReader sr(v.c_str());
    lucene::analysis::standard::StandardAnalyzer a;
    lucene::analysis::TokenStream* ts = a.tokenStream(name, &sr);
    lucene::analysis::Token* to = ts->next();
    const wchar_t *tv;
    if (to) {
        tv = to->termText();
    } else {
        tv = v.c_str();
    }
    Term* t = _CLNEW Term(name, tv);
    if (to) {
        _CLDELETE(to);
    }
    _CLDELETE(ts);
    return t;
}
Term*
CLuceneIndexReader::Private::createKeywordTerm(const wchar_t* name,
        const string& value) {
    wstring v = utf8toucs2(value);
    Term* t = _CLNEW Term(name, v.c_str());
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
    Term* t;
    const string& val = query.term().string();
    switch (query.type()) {
    case Strigi::Query::LessThan:
          t = createTerm(fieldname.c_str(), val.c_str());
          q = _CLNEW RangeQuery(0, t, false);
          break;
    case Strigi::Query::LessThanEquals:
          t = createTerm(fieldname.c_str(), query.term().string());
          q = _CLNEW RangeQuery(0, t, true);
          break;
    case Strigi::Query::GreaterThan:
          t = createTerm(fieldname.c_str(), query.term().string());
          q = _CLNEW RangeQuery(t, 0, false);
          break;
    case Strigi::Query::GreaterThanEquals:
          t = createTerm(fieldname.c_str(), query.term().string());
          q = _CLNEW RangeQuery(t, 0, true);
          break;
    case Strigi::Query::Keyword:
          t = createKeywordTerm(fieldname.c_str(), query.term().string());
          q = _CLNEW TermQuery(t);
          break;
    case Strigi::Query::Contains:
          t = createWildCardTerm(fieldname.c_str(), "*" + val + "*");
          q = _CLNEW WildcardQuery(t);
          break;
    case Strigi::Query::StartsWith:
          t = createWildCardTerm(fieldname.c_str(), val + "*");
          q = _CLNEW WildcardQuery(t);
          break;
    case Strigi::Query::Equals:
    default:
          if (strpbrk(val.c_str(), "*?")) {
               t = createWildCardTerm(fieldname.c_str(), val);
               q = _CLNEW WildcardQuery(t);
          } else {
               t = createTerm(fieldname.c_str(), val);
               q = _CLNEW TermQuery(t);
          }
    }
    _CLDECDELETE(t);
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
    if (!checkReader()) return -1;
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
    if (!checkReader()) {
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
        DocumentFieldEnumeration* e = d->fields();
        while (e->hasMoreElements()) {
            Field* f = e->nextElement();
            Private::addField(f, doc);
        }
        results.push_back(doc);
        _CLDELETE(e);
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
    int pos = 0;
    int maxDoc = reader->maxDoc();
    for (int i=0; i<off; i++) {
        while (pos < maxDoc && reader->isDeleted(pos)) pos++;
        if (pos == maxDoc) return;
        pos++;
    }
    if (max < 0) max = 0;
    result.resize(max);
    Document* d = new Document();
    for (int i = 0; i < max && pos < maxDoc; ++i) {
        while (pos < maxDoc && reader->isDeleted(pos)) pos++;
        d->clear();
        if (pos == maxDoc || !reader->document(pos++, d)) {
            continue;
        }
        
        vector<Variant>& doc = result[i];
        doc.clear();
        doc.resize(fullFields.size());

        DocumentFieldEnumeration* e = d->fields();
        while (e->hasMoreElements()) {
            Field* field = e->nextElement();
            string name(wchartoutf8(field->name()));
            for (uint j = 0; j < fullFields.size(); ++j) {
                if (fullFields[j] == name) {
                    doc[j] = p->getFieldValue(field, types[j]);
                }
            }
        }
        _CLDELETE(e);
    }
    delete d;
}
void
CLuceneIndexReader::getHits(const Strigi::Query& q,
        const std::vector<std::string>& fields,
        const std::vector<Strigi::Variant::Type>& types,
        std::vector<std::vector<Strigi::Variant> >& result, int off, int max) {
    result.clear();
    if (!checkReader() || types.size() < fields.size()) {
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

        DocumentFieldEnumeration* e = d->fields();
        while (e->hasMoreElements()) {
            Field* field = e->nextElement();
            string name(wchartoutf8(field->name()));
            for (uint j = 0; j < fullFields.size(); ++j) {
                if (fullFields[j] == name) {
                    doc[j] = p->getFieldValue(field, types[j]);
                }
            }
        }
        _CLDELETE(e);
    }
    if (hits) {
        _CLDELETE(hits);
    }
    searcher.close();
    _CLDELETE(bq);
}
int32_t
CLuceneIndexReader::countDocuments() {
    if (!checkReader(true)) return -1;
    if (doccount == -1) {
        doccount = reader->numDocs();
    }
    return doccount;
}
int32_t
CLuceneIndexReader::countWords() {
    if (!checkReader()) return -1;
    if (wordcount == -1) {
        if (reader) {
            wordcount = 0;
            lucene::index::TermEnum *terms = reader->terms();
            while (terms->next()) wordcount++;
            _CLDELETE(terms);
        }
    }
    return wordcount;
}
int64_t
CLuceneIndexReader::indexSize() {
    return manager->indexSize();
}
int64_t
CLuceneIndexReader::documentId(const string& uri) {
    if (!checkReader()) return -1;
    int64_t id = -1;

    Term term(mapId(Private::systemlocation()), utf8toucs2( uri ).c_str());
    TermDocs* docs = reader->termDocs(&term);
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
    if (!checkReader(true)) return 0;
    time_t mtime = 0;
    Document *d = reader->document((int32_t)docid);
    if (d) {
        const TCHAR* v = d->get(Private::mtime());
        mtime = atoi(wchartoutf8( v ).c_str());
        delete d;
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
    if (!checkReader()) {
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
    if (!checkReader()) {
        return s;
    }
    TCHAR** names = reader->getFieldNames();
    if (names == 0) return s;
    TCHAR** n = names;
    while (*n) {
        string str(wchartoutf8(*n));
        s.push_back(str);
        _CLDELETE_ARRAY(*n);
        n++;
    }
    _CLDELETE_ARRAY(names);
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
    Term* lastTerm = 0;
    for (i = fn.begin(); i != fn.end() && s.size() << max; ++i) {
         wstring fieldname(utf8toucs2(*i));
         Term term(fieldname.c_str(), prefix.c_str());
         TermEnum* enumerator = reader->terms(&term);
         do {
             lastTerm = enumerator->term(false);
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

    vector<string> k;
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
    if ( !checkReader(true) ) {
        return;
    }
    // build a query
    Term* t = Private::createKeywordTerm(Private::parentlocation(),
        parent);
    Query* q = _CLNEW TermQuery(t);
    _CLDECDELETE(t);
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
cerr << "strigispecial " << command << endl;
    // we are going to count the size of each of the fields in this index
    // this requires that we loop through all fields
    lucene::index::TermEnum* terms = reader.reader->terms();

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

    int32_t max = reader.reader->numDocs();
    for (int32_t i=0; i < max; ++i) {
        lucene::document::Document* d = reader.reader->document(i);
        lucene::document::DocumentFieldEnumeration* e = d->fields();
        while (e->hasMoreElements()) {
            Field* f = e->nextElement();
            if (f->isStored()) {
                total += wcslen(f->stringValue());
            }
        }
        delete d;
    }
    cerr << "total" << '\t' << total << endl;
    return r;
}

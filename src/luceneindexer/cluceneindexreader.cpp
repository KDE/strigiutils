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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "cluceneindexreader.h"
#include "strigiconfig.h"
#include "query.h"
#include "queryparser.h"
#include "textutils.h"
#include "cluceneindexmanager.h"
#include "timeofday.h"
#include <CLucene/search/QueryFilter.h>
#include <CLucene/index/Terms.h>
#include <sstream>
#include <iostream>

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
    CLuceneIndexReader& reader;
    Private(CLuceneIndexReader& r) :reader(r) {}

    static Term* createTerm(const wchar_t* name, const string& value);
    static Term* createWildCardTerm(const wchar_t* name, const string& value);
    Query* createQuery(const Strigi::Query& query);
    Query* createSimpleQuery(const Strigi::Query& query);
    static Query* createSingleFieldQuery(const string& field,
        const Strigi::Query& query);
    Query* createNoFieldQuery(const Strigi::Query& query);
    Query* createMultiFieldQuery(const Strigi::Query& query);
    BooleanQuery* createBooleanQuery(const Strigi::Query& query);
    static void addField(lucene::document::Field* field, IndexedDocument&);
};

CLuceneIndexReader::CLuceneIndexReader(CLuceneIndexManager* m,
    const string& dir) :manager(m), p(new Private(*this)), dbdir(dir), otime(0),
        reader(0) {
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
//        printf("reader at %s\n", dbdir.c_str());
        reader = lucene::index::IndexReader::open(dbdir.c_str());
    } catch (CLuceneError& err) {
        printf("could not create reader: %s\n", err.what());
        reader = 0;
    }
}
void
CLuceneIndexReader::closeReader() {
    if (reader == 0) return;
    try {
        reader->close();
    } catch (CLuceneError& err) {
        printf("could not close clucene: %s\n", err.what());
    }
    delete reader;
    reader = 0;
}
bool
CLuceneIndexReader::checkReader(bool enforceCurrent) {
    if (manager->indexMTime() > otime) {
        struct timeval t;
        gettimeofday(&t, 0);
        if (enforceCurrent || t.tv_sec-otime > 60) {
            otime = t.tv_sec;
            closeReader();
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
        addMapping(_T(""), _T("content"));
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
    case 0:  return createNoFieldQuery(query);
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
//    string n(wchartoutf8(name));
//    printf("%s\t%s\n", n.c_str(), value.c_str());
    if (wcscmp(name, L"content") == 0) {
        doc.fragment = value;
    } else if (wcscmp(name, L"system.location") == 0) {
        doc.uri = value;
    } else if (wcscmp(name, L"content.mime_type") == 0) {
        doc.mimetype = value;
    } else if (wcscmp(name, L"system.last_modified_time") == 0) {
        doc.mtime=atol(value.c_str());
    } else if (wcscmp(name, L"system.size") == 0) {
        string size = value;
        doc.size = atoi(size.c_str());
    } else {
        doc.properties.insert(make_pair<const string, string>(
            wchartoutf8(name), value));
    }
}
int32_t
CLuceneIndexReader::countHits(const Strigi::Query& q) {
    if (!checkReader()) return -1;
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
        printf("could not query: %s\n", err.what());
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
    Query* bq = p->createQuery(q);
    IndexSearcher searcher(reader);
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(bq);
        s = hits->length();
    } catch (CLuceneError& err) {
        printf("could not query: %s\n", err.what());
    }
    if (off < 0) off = 0;
    max += off;
    if (max < 0) max = s;
    if (max > s) max = s;
    results.reserve(max-off);
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
map<string, time_t>
CLuceneIndexReader::files(char depth) {
    map<string, time_t> files;
    if (!checkReader()) {
        return files;
    }

    const TCHAR* mtime = mapId(_T("system.last_modified_time"));
    char cstr[CL_MAX_DIR];
    if (depth >= 0) {
        TCHAR tstr[CL_MAX_DIR];
        snprintf(cstr, CL_MAX_DIR, "%i", depth);
        STRCPY_AtoT(tstr, cstr, CL_MAX_DIR);
        Term term(mapId(_T("system.depth")), tstr);
        TermDocs* docs = reader->termDocs(&term);
        while (docs->next()) {
            Document* d = reader->document(docs->doc());

            const TCHAR* v = d->get(mtime);
            STRCPY_TtoA(cstr, v, CL_MAX_DIR);
            time_t mtime = atoi(cstr);
            v = d->get(_T("system.location"));
            STRCPY_TtoA(cstr, v, CL_MAX_DIR);
            files[cstr] = mtime;

            _CLDELETE(d);
        }
        _CLDELETE(docs);
    } else {
        int32_t max = reader->maxDoc();
        for (int32_t i = 0; i < max; ++i) {
            if (!reader->isDeleted(i)) {
                Document* d = reader->document(i);
                const TCHAR* v = d->get(mtime);
                STRCPY_TtoA(cstr, v, CL_MAX_DIR);
                time_t mtime = atoi(cstr);
                v = d->get(_T("system.location"));
                STRCPY_TtoA(cstr, v, CL_MAX_DIR);
                files[cstr] = mtime;
                _CLDELETE(d);
            }
        }
    }
    return files;
}
int32_t
CLuceneIndexReader::countDocuments() {
    if (!checkReader()) return -1;
    if (doccount == -1) {
        doccount = manager->docCount();
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

    TCHAR tstr[CL_MAX_DIR];
    STRCPY_AtoT(tstr, uri.c_str(), CL_MAX_DIR);
    Term term(mapId(_T("system.location")), tstr);
    TermDocs* docs = reader->termDocs(&term);
    if (docs->next()) {
        id = docs->doc();
    }
    _CLDELETE(docs);

    if (id != -1 && reader->isDeleted(id)) {
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
    Document *d = reader->document(docid);
    if (d) {
        char cstr[CL_MAX_DIR];
        const TCHAR* v = d->get(mapId(_T("system.last_modified_time")));
        STRCPY_TtoA(cstr, v, CL_MAX_DIR);
        mtime = atoi(cstr);
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
        printf("could not query: %s\n", err.what());
    }
    char cstr[CL_MAX_DIR];
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
            STRCPY_TtoA(cstr, v, CL_MAX_DIR);
            int val = (int)strtol(cstr, &end, 10);
            if (end == cstr || *end != 0) {
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
    if (fieldname == "system.last_modified_time" || labeltype == "time") {
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
    TCHAR** n = names;
    while (*n) {
        string str(wchartoutf8(*n));
        s.push_back(str);
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
    uint32_t prefixLen = prefix.length();
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
                         || _tcsncmp(lastTerm->text(), prefixtext, prefixLen)
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

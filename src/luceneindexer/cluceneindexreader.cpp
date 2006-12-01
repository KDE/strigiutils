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
#include "query.h"
#include "textutils.h"
#include <CLucene.h>
#include "cluceneindexreader.h"
#include "cluceneindexmanager.h"
#include "timeofday.h"
#include <CLucene/search/QueryFilter.h>
#include <CLucene/index/Terms.h>
#include <sstream>

using lucene::search::Hits;
using lucene::search::IndexSearcher;
using lucene::document::Document;
using lucene::document::Field;
using lucene::index::Term;
using lucene::index::TermDocs;
using lucene::search::TermQuery;
using lucene::search::WildcardQuery;
using lucene::search::BooleanQuery;
using lucene::search::RangeQuery;
using lucene::search::QueryFilter;
using lucene::search::HitCollector;
using lucene::util::BitSet;
using lucene::document::DocumentFieldEnumeration;
using namespace jstreams;

class HitCounter : public HitCollector {
private:
    int32_t count;
    void collect (const int32_t doc, const float_t score) { count++; }
public:
    HitCounter() :count(0) {}
    int32_t getCount() const { return count; }
};

class CLuceneIndexReader::Private {
public:
    static lucene::index::Term* createTerm(const wchar_t* name,
        const std::string& value);
    static lucene::index::Term* createWildCardTerm(const wchar_t* name,
        const std::string& value);
    static void createBooleanQuery(const jstreams::Query& query,
        lucene::search::BooleanQuery& bq);
    static void addField(lucene::document::Field* field,
        jstreams::IndexedDocument&);
};

CLuceneIndexReader::CLuceneIndexReader(CLuceneIndexManager* m,
    const string& dir) :manager(m), dbdir(dir), otime(0), reader(0) {
    openReader();
}

CLuceneIndexReader::~CLuceneIndexReader() {
    closeReader();
}
void
CLuceneIndexReader::openReader() {
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
    if (manager->getIndexMTime() > otime) {
        struct timeval t;
        gettimeofday(&t, 0);
        if (enforceCurrent || t.tv_sec-otime > 60) {
            fprintf(stderr, "reopening reader.\n");
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
std::wstring
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
void
CLuceneIndexReader::Private::createBooleanQuery(const Query& query, BooleanQuery& bq) {
    lucene::analysis::standard::StandardAnalyzer a;
    // add the attributes
    const map<string, set<string> >& includes = query.getIncludes();
    map<string, set<string> >::const_iterator i;
    set<string>::const_iterator j;
    for (i = includes.begin(); i != includes.end(); ++i) {
        wstring mappedId = mapId(i->first.c_str());
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            lucene::search::Query* tq;
            Term* t = 0;
            if (j->length() > 0 && (*j)[0] == '<') {
                t = createTerm(mappedId.c_str(), (*j).substr(1));
                tq = _CLNEW RangeQuery(0, t, false);
            } else if (j->length() > 0 && (*j)[0] == '>') {
                t = createTerm(mappedId.c_str(), (*j).substr(1));
                tq = _CLNEW RangeQuery(t, 0, false);
            } else {
                if (strpbrk(j->c_str(), "*?")) {
                    t = createWildCardTerm(mappedId.c_str(), *j);
                    tq = _CLNEW WildcardQuery(t);
                } else {
                    t = createTerm(mappedId.c_str(), *j);
                    tq = _CLNEW TermQuery(t);
                }
            }
            if (t) _CLDECDELETE(t);
            bq.add(tq, true, true, false);
        }
    }
    const map<string, set<string> >& excludes = query.getExcludes();
    for (i = excludes.begin(); i != excludes.end(); ++i) {
        wstring mappedId = mapId(i->first.c_str());
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            lucene::search::Query* tq;
            bool wildcard = strpbrk(j->c_str(), "*?")!=NULL;
            Term* t;
            if (wildcard) {
                t = createWildCardTerm(mappedId.c_str(), *j);
                tq = _CLNEW WildcardQuery(t);
            } else {
                t = createTerm(mappedId.c_str(), *j);
                tq = _CLNEW TermQuery(t);
            }
            _CLDECDELETE(t);
            bq.add(tq, true, false, true);
        }
    }
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
    } else if (wcscmp(name, L"path") == 0) {
        doc.uri = value;
    } else if (wcscmp(name, L"mimetype") == 0) {
        doc.mimetype = value;
    } else if (wcscmp(name, L"mtime") == 0) {
        doc.mtime=atol(value.c_str());
    } else if (wcscmp(name, L"size") == 0) {
        string size = value;
        doc.size = atoi(size.c_str());
    } else {
        doc.properties.insert(make_pair(wchartoutf8(name), value));
    }
}
int32_t
CLuceneIndexReader::countHits(const Query& q) {
    if (!checkReader()) return -1;
    BooleanQuery bq;
    Private::createBooleanQuery(q, bq);
    if (reader == 0) {
        return 0;
    }
    IndexSearcher searcher(reader);
    std::vector<IndexedDocument> results;
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(&bq);
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
        s = counter.getCount();

        printf("counted %i hits\n", count);
        // try to do a constant score query
        //QueryFilter* filter = _CLNEW QueryFilter(&bq);
        ConstantScoreQuery csq(filter);*/
        printf("could not query: %s\n", err.what());
    }
    if (hits) {
        delete hits;
    }
    searcher.close();
    return s;
}
std::vector<IndexedDocument>
CLuceneIndexReader::query(const Query& q) {
    BooleanQuery bq;
    Private::createBooleanQuery(q, bq);
    std::vector<IndexedDocument> results;
    if (!checkReader()) {
        return results;
    }
    IndexSearcher searcher(reader);
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(&bq);
        s = hits->length();
    } catch (CLuceneError& err) {
        printf("could not query: %s\n", err.what());
    }
    int off = q.getOffset();
    if (off < 0) off = 0;
    int max = q.getMax() + off;
    if (max < 0) max = s;
    if (max > s) max = s;
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
    return results;
}
std::map<std::string, time_t>
CLuceneIndexReader::getFiles(char depth) {
    std::map<std::string, time_t> files;
    if (!checkReader()) {
        return files;
    }

    TCHAR tstr[CL_MAX_DIR];
    char cstr[CL_MAX_DIR];
    snprintf(cstr, CL_MAX_DIR, "%i", depth);
    STRCPY_AtoT(tstr, cstr, CL_MAX_DIR);

    Term term(mapId(_T("depth")), tstr);
    TermDocs* docs = reader->termDocs(&term);
    while ( docs->next() ){
        Document *d = reader->document(docs->doc());

        const TCHAR* v = d->get(mapId(_T("mtime")));
        STRCPY_TtoA(cstr, v, CL_MAX_DIR);
        time_t mtime = atoi(cstr);
        v = d->get(_T("path"));
        STRCPY_TtoA(cstr, v, CL_MAX_DIR);
        files[cstr] = mtime;

        _CLDELETE(d);
    }
    _CLDELETE(docs);
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
CLuceneIndexReader::getIndexSize() {
    return manager->getIndexSize();
}
int64_t
CLuceneIndexReader::getDocumentId(const std::string& uri) {
    if (!checkReader()) return -1;
    int64_t id = -1;

    TCHAR tstr[CL_MAX_DIR];
    STRCPY_AtoT(tstr, uri.c_str(), CL_MAX_DIR);
    Term term(mapId(_T("path")), tstr);
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
CLuceneIndexReader::getMTime(int64_t docid) {
    if (docid < 0) return 0;
    if (!checkReader(true)) return 0;
    time_t mtime = 0;
    Document *d = reader->document(docid);
    if (d) {
        char cstr[CL_MAX_DIR];
        const TCHAR* v = d->get(mapId(_T("mtime")));
        STRCPY_TtoA(cstr, v, CL_MAX_DIR);
        mtime = atoi(cstr);
        delete d;
    }
    return mtime;
}

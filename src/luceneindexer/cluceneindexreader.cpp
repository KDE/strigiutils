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
#include "cluceneindexreader.h"
#include "cluceneindexmanager.h"
#include <CLucene.h>
#include <CLucene/search/QueryFilter.h>
#include <sstream>

using lucene::search::Hits;
using lucene::search::IndexSearcher;
using lucene::document::Document;
using lucene::document::Field;
using lucene::index::Term;
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
    static lucene::index::Term* createTerm(const std::string& name,
        const std::string& value);
    static void createBooleanQuery(const jstreams::Query& query,
        lucene::search::BooleanQuery& bq);
    static void addField(lucene::document::Field* field,
        jstreams::IndexedDocument&);
};

CLuceneIndexReader::CLuceneIndexReader(CLuceneIndexManager* m)
        :manager(m), countversion(-1) {
}

CLuceneIndexReader::~CLuceneIndexReader() {
}

const char*
CLuceneIndexReader::mapId(const std::string& id) {
    if (id == "") return "content";
    return id.c_str();
}
Term*
CLuceneIndexReader::Private::createTerm(const string& name, const string& value) {
#ifndef _CL_HAVE_WCSLEN
    return _CLNEW Term(name.c_str(), value.c_str());
#else
#endif
    wstring n = utf8toucs2(name);
    wstring v = utf8toucs2(value);
    Term* t = _CLNEW Term(n.c_str(), v.c_str());
    return t;
}
void
CLuceneIndexReader::Private::createBooleanQuery(const Query& query, BooleanQuery& bq) {
    // add the attributes
    const map<string, set<string> >& includes = query.getIncludes();
    map<string, set<string> >::const_iterator i;
    set<string>::const_iterator j;
    for (i = includes.begin(); i != includes.end(); ++i) {
        string id = mapId(i->first);
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            lucene::search::Query* tq;
            Term* t;
            if (j->length() > 0 && (*j)[0] == '<') {
                t = createTerm(mapId(i->first), j->substr(1));
                tq = _CLNEW RangeQuery(0, t, false);
            } else if (j->length() > 0 && (*j)[0] == '>') {
                t = createTerm(mapId(i->first), j->substr(1));
                tq = _CLNEW RangeQuery(t, 0, false);
            } else {
                t = createTerm(mapId(i->first), *j);
                if (strpbrk(j->c_str(), "*?")) {
                    tq = _CLNEW WildcardQuery(t);
                } else {
                    tq = _CLNEW TermQuery(t);
                }
            }
            _CLDECDELETE(t);
            bq.add(tq, true, true, false);
        }
    }
    const map<string, set<string> >& excludes = query.getExcludes();
    for (i = excludes.begin(); i != excludes.end(); ++i) {
        string id = mapId(i->first);
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            Term* t = createTerm(mapId(i->first), *j);
            lucene::search::Query* tq;
            bool wildcard = strpbrk(j->c_str(), "*?")!=NULL;
            if (wildcard) {
                tq = _CLNEW WildcardQuery(t);
            } else {
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
    if (wcscmp(name, L"content") == 0) {
        doc.fragment = value;
    } else if (wcscmp(name, L"path") == 0) {
        doc.uri = value;
    } else if (wcscmp(name, L"mimetype") == 0) {
        doc.mimetype = value;
    } else if (wcscmp(name, L"mtime") == 0) {
        istringstream iss(value);
        iss >> doc.mtime;
    } else if (wcscmp(name, L"size") == 0) {
        string size = value;
        doc.size = atoi(size.c_str());
    } else {
        doc.properties.insert(make_pair(wchartoutf8(name), value));
    }
}
int32_t
CLuceneIndexReader::countHits(const Query& q) {
    BooleanQuery bq;
    Private::createBooleanQuery(q, bq);
    lucene::index::IndexReader* reader = manager->refReader();
    if (reader == 0) {
        manager->derefReader();
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
    manager->derefReader();
    return s;
}
std::vector<IndexedDocument>
CLuceneIndexReader::query(const Query& q) {
    BooleanQuery bq;
    Private::createBooleanQuery(q, bq);
    std::vector<IndexedDocument> results;
    lucene::index::IndexReader* reader = manager->refReader();
    if (reader == 0) {
        manager->derefReader();
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
    manager->derefReader();
    return results;
}
std::map<std::string, time_t>
CLuceneIndexReader::getFiles(char depth) {
    std::map<std::string, time_t> files;
    lucene::index::IndexReader* reader = manager->refReader();
    if (reader == 0) {
        manager->derefReader();
        return files;
    }
    IndexSearcher searcher(reader);
    TCHAR tstr[CL_MAX_DIR];
    char cstr[CL_MAX_DIR];
    snprintf(cstr, CL_MAX_DIR, "%i", depth);
    STRCPY_AtoT(tstr, cstr, CL_MAX_DIR);
    Term* term = _CLNEW Term(_T("depth"), tstr);
    TermQuery* termquery = _CLNEW TermQuery(term);
    Hits *hits = searcher.search(termquery);
    int s = hits->length();
    for (int i = 0; i < s; ++i) {
        Document *d = &hits->doc(i);
        const TCHAR* v = d->get(_T("mtime"));
        STRCPY_TtoA(cstr, v, CL_MAX_DIR);
        time_t mtime = atoi(cstr);
        v = d->get(_T("path"));
        files[wchartoutf8(v)] = mtime;
    }
    searcher.close();
    _CLDELETE(hits);
    _CLDELETE(termquery);
    _CLDECDELETE(term);
    manager->derefReader();
    return files;
}
int32_t
CLuceneIndexReader::countDocuments() {
    return manager->docCount();
}
int32_t
CLuceneIndexReader::countWords() {
    if (manager->getVersion() == countversion) {
        return count;
    }
    count = 0;
    countversion = manager->getVersion();
    lucene::index::IndexReader* reader = manager->refReader();
    if (reader) {
        lucene::index::TermEnum *terms = reader->terms();
        while (terms->next()) count++;
        _CLDELETE(terms);
    }
    manager->derefReader();
    return count;
}
int64_t
CLuceneIndexReader::getIndexSize() {
    return manager->getIndexSize();
}
int64_t
CLuceneIndexReader::getDocumentId(const std::string& uri) {
    lucene::index::IndexReader* reader = manager->refReader();
    IndexSearcher searcher(reader);
    wstring tstr(utf8toucs2(uri));
    Term* term = _CLNEW Term(_T("path"), tstr.c_str());
    TermQuery* termquery = _CLNEW TermQuery(term);
    Hits *hits = searcher.search(termquery);
    int s = hits->length();
    int64_t id = -1;
    if (s == 1) {
        id = hits->id(0);
    }
    searcher.close();
    _CLDELETE(hits);
    _CLDELETE(termquery);
    _CLDECDELETE(term);
    manager->derefReader();
    return id;
}
/**
 * Retrieve the mtime of the document with id @docid. If this document
 * is not in the index, the time 0 is returned.
 **/
time_t
CLuceneIndexReader::getMTime(int64_t docid) {
    if (docid < 0) return 0;
    lucene::index::IndexReader* reader = manager->refReader();
    time_t mtime = 0;
    Document *d = reader->document(docid);
    if (d) {
        char cstr[CL_MAX_DIR];
        const TCHAR* v = d->get(_T("mtime"));
        STRCPY_TtoA(cstr, v, CL_MAX_DIR);
        mtime = atoi(cstr);
    }
    manager->derefReader();
    return mtime;
}

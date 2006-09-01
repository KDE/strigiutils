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
#include "cluceneindexreader.h"
#include "cluceneindexmanager.h"
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
using lucene::search::QueryFilter;
using lucene::document::DocumentFieldEnumeration;
using namespace jstreams;

CLuceneIndexReader::~CLuceneIndexReader() {
}

const char*
CLuceneIndexReader::mapId(const std::string& id) {
    if (id == "") return "content";
    return id.c_str();
}
Term*
CLuceneIndexReader::createTerm(const string& name, const string& value) {
#ifndef _CL_HAVE_WCSLEN
    return _CLNEW Term(name.c_str(), value.c_str());
#else
#endif
    TCHAR* n = new TCHAR[name.length()+1];
    STRCPY_AtoT(n, name.c_str(), name.length()+1);
    TCHAR* v = new TCHAR[value.length()+1];
    STRCPY_AtoT(v, value.c_str(), value.length()+1);
    Term* t = _CLNEW Term(n, v);
    delete [] n;
    delete [] v;
    return t;
}
void
CLuceneIndexReader::createBooleanQuery(const Query& query, BooleanQuery& bq) {
    // add the attributes
    const map<string, set<string> >& includes = query.getIncludes();
    map<string, set<string> >::const_iterator i;
    set<string>::const_iterator j;
    for (i = includes.begin(); i != includes.end(); ++i) {
        string id = mapId(i->first);
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            Term* t = createTerm(mapId(i->first), *j);
            lucene::search::Query* tq;
            bool wildcard = strpbrk(j->c_str(), "*?");
            if (wildcard) {
                tq = _CLNEW WildcardQuery(t);
            } else {
                tq = _CLNEW TermQuery(t);
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
            bool wildcard = strpbrk(j->c_str(), "*?");
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
std::string
CLuceneIndexReader::convertValue(const TCHAR* v) {
    if (v == 0) return "";
    char after[CL_MAX_DIR];
    STRCPY_TtoA(after, v, CL_MAX_DIR);
    after[CL_MAX_DIR-1] = '\0';
    // remove newlines
    char *p = after;
    while (true) {
        if (*p == '\0') break;
        if (*p == '\n' || *p == '\r') *p = ' ';
        p++;
    }
    return after;
}
void
CLuceneIndexReader::addField(lucene::document::Field* field,
        IndexedDocument& doc) {
    const TCHAR* value = field->stringValue();
    if (value == 0) return;
    char name[CL_MAX_DIR];
    STRCPY_TtoA(name, field->name(), CL_MAX_DIR);
    if (strcmp(name, "content") == 0) {
        doc.fragment = convertValue(value);
    } else if (strcmp(name, "path") == 0) {
        doc.uri = convertValue(value);
    } else if (strcmp(name, "mimetype") == 0) {
        doc.mimetype = convertValue(value);
    } else if (strcmp(name, "mtime") == 0) {
        istringstream iss(convertValue(value));
        iss >> doc.mtime;
    } else if (strcmp(name, "size") == 0) {
        string size = convertValue(value);
        doc.size = atoi(size.c_str());
    } else {
        doc.properties[name] = convertValue(value);
    }
}
int32_t
CLuceneIndexReader::countHits(const Query& q) {
    BooleanQuery bq;
    createBooleanQuery(q, bq);
    lucene::index::IndexReader* reader = manager->refReader();
    IndexSearcher searcher(reader);
    std::vector<IndexedDocument> results;
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(&bq);
        s = hits->length();
    } catch (CLuceneError& err) {
        // try to do a constant score query
        //QueryFilter* filter = _CLNEW QueryFilter(&bq);
        //ConstantScoreQuery csq(filter);
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
    createBooleanQuery(q, bq);
    lucene::index::IndexReader* reader = manager->refReader();
    IndexSearcher searcher(reader);
    std::vector<IndexedDocument> results;
    TCHAR tf[CL_MAX_DIR];
    Hits* hits = 0;
    int s = 0;
    try {
        hits = searcher.search(&bq);
        s = hits->length();
    } catch (CLuceneError& err) {
        printf("could not query: %s\n", err.what());
    }
    STRCPY_AtoT(tf, "path", CL_MAX_DIR);
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
            addField(f, doc);
        }
        results.push_back(doc);
    }
    if (hits) {
        delete hits;
    }
    searcher.close();
    manager->derefReader();
    return results;
}
std::map<std::string, time_t>
CLuceneIndexReader::getFiles(char depth) {
    std::map<std::string, time_t> files;
    lucene::index::IndexReader* reader = manager->refReader();
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
        STRCPY_TtoA(cstr, v, CL_MAX_DIR);
        files[cstr] = mtime;
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
    lucene::index::IndexReader* reader = manager->refReader();
    count = 0;
    countversion = manager->getVersion();
    lucene::index::TermEnum *terms = reader->terms();
    while (terms->next()) count++;
    _CLDELETE(terms);
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
    TCHAR tstr[CL_MAX_DIR];
    STRCPY_AtoT(tstr, uri.c_str(), CL_MAX_DIR);
    Term* term = _CLNEW Term(_T("path"), tstr);
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

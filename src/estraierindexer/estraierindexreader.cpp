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
#include "estraierindexreader.h"
#include "estraierindexmanager.h"
#include "query.h"
#include <estraier.h>
#include <set>
#include <sstream>
#include <assert.h>
using namespace std;
using namespace Strigi;

EstraierIndexReader::EstraierIndexReader(EstraierIndexManager* m)
    : manager(m) {
}
EstraierIndexReader::~EstraierIndexReader() {
}
ESTCOND*
EstraierIndexReader::createCondition(const Strigi::Query& query) {
    // build the phrase string

    // write the part of the query that matches the document context
    string phrase;
    set<string> terms;
    const map<string, set<string> >& includes = query.getIncludes();
    map<string, set<string> >::const_iterator i = includes.find("");
    if (i != includes.end()) terms = i->second;
    set<string>::const_iterator j;
    for (j = terms.begin(); j != terms.end(); ++j) {
        if (phrase.length() > 0) {
            phrase += " AND ";
        }
        phrase += *j;
    }
    terms.clear();

    // add the part of the query that excludes terms
    const map<string, set<string> >& excludes = query.getExcludes();
    i = excludes.find("");
    if (i != excludes.end()) terms = i->second;
    for (j = terms.begin(); j != terms.end(); ++j) {
        if (phrase.length() > 0) {
            phrase += " ANDNOT ";
        }
        phrase += *j;
    }
    ESTCOND* cond = est_cond_new();
    printf("%s", phrase.c_str());
    if (phrase.length() > 0) {
        est_cond_set_phrase(cond, phrase.c_str());
    }

    // add the attributes
    for (i = includes.begin(); i != includes.end(); ++i) {
        if (i->first.length() == 0) continue;
        string id = mapId(i->first);
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            ostringstream att;
            if (j->length() > 0 && (*j)[0] == '<') {
                att << id << " NUMLT " << j->substr(1);
            } else if (j->length() > 0 && (*j)[0] == '>') {
                att << id << " NUMGT " << j->substr(1);
            } else {
                att << id << " STRINC " << *j;
            }
            printf(" && %s", att.str().c_str());
            est_cond_add_attr(cond, att.str().c_str());
        }
    }
    for (i = excludes.begin(); i != excludes.end(); ++i) {
        if (i->first.length() == 0) continue;
        string id = mapId(i->first);
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            string att = id + " !STRINC " + *j;
            printf(" && %s", att.c_str());
            est_cond_add_attr(cond, att.c_str());
        }
    }
    printf("\n");

    return cond;
}
const char*
EstraierIndexReader::mapId(const string& id) {
    if (id == "path") return "@uri";
    if (id == "mtime") return "@mdate";
    if (id == "title") return "@title";
    if (id == "size") return "@size";
    return id.c_str();
}
string
EstraierIndexReader::getFragment(ESTDOC* doc, const Query& query) {
    string fragment;
    const CBLIST *list = est_doc_texts(doc);
    for (int j = 0; j < cblistnum(list); ++j) {
        if (j) fragment += ' ';
        fragment += cblistval(list, j, 0);
    }
//    char* f = est_doc_cat_texts(doc);
//    string fragment = f;
//    free(f);
    return fragment;
}
int32_t
EstraierIndexReader::countHits(const Strigi::Query& query) {
    ESTCOND* cond = createCondition(query);
    int n;
    int* ids;

    ESTDB* db = manager->ref();
    ids = est_db_search(db, cond, &n, NULL);
    int r = est_db_error(db);
    if (r != ESTENOERR && r != ESTENOITEM) {
        fprintf(stderr, "%s\n", est_err_msg(r));
    }
    manager->deref();
    est_cond_delete(cond);
    free(ids);
    return n;
}
vector<IndexedDocument>
EstraierIndexReader::query(const Query& query) {
    ESTCOND* cond = createCondition(query);
    est_cond_set_max(cond, 10);
    int n;
    int* ids;

    ESTDB* db = manager->ref();
    ids = est_db_search(db, cond, &n, NULL);

    vector<IndexedDocument> results;
    for (int i=0; i<n; ++i) {
        int id = ids[i];
        IndexedDocument doc;
        doc.score = est_cond_score(cond, i);
        ESTDOC* d = est_db_get_doc(db, id, ESTGDNOKWD);
        if (!d) continue;
        doc.fragment = getFragment(d, query);
        CBLIST* atts = est_doc_attr_names(d);
        for (int j = 0; j < cblistnum(atts); ++j) {
            const char* name = cblistval(atts, j, 0);
            const char* value = est_doc_attr(d, name);
            if (*name == '@') name++;
            if (strcmp(name, "uri") == 0) {
                doc.uri = value;
            } else if (strcmp(name, "mimetype") == 0) {
                doc.mimetype = value;
            } else if (strcmp(name, "mdate") == 0) {
                istringstream iss(value);
                iss >> doc.mtime;
            } else if (strcmp(name, "size") == 0) {
                istringstream iss(value);
                iss >> doc.size;
            } else {
                doc.properties.insert(make_pair(name,value));
            }
        }
        cblistclose(atts);
        results.push_back(doc);
    }
    manager->deref();

    // clean up
    est_cond_delete(cond);
    free(ids);
    return results;
}
map<string, time_t>
EstraierIndexReader::getFiles(char depth) {
    map<string, time_t> files;
    ESTCOND* cond = est_cond_new();
    string q = "depth NUMEQ 0";
    est_cond_add_attr(cond, q.c_str());
    int n;
    int* ids;

    ESTDB* db = manager->ref();
    ids = est_db_search(db, cond, &n, NULL);

    for (int i=0; i<n; ++i) {
        char* uri = est_db_get_doc_attr(db, ids[i], "@uri");
        char* mdate = est_db_get_doc_attr(db, ids[i], "@mdate");
        time_t md = atoi(mdate);
        assert(uri);
        files[uri] = md;
        free(uri);
        free(mdate);
    }
    manager->deref();

    // clean up
    est_cond_delete(cond);
    free(ids);
    return files;
}
int32_t
EstraierIndexReader::countDocuments() {
    ESTDB* db = manager->ref();
    int count = est_db_doc_num(db);
    manager->deref();
    return count;
}
int32_t
EstraierIndexReader::countWords() {
    ESTDB* db = manager->ref();
    int count = est_db_word_num(db);
    manager->deref();
    return count;
}
int64_t
EstraierIndexReader::getIndexSize() {
    ESTDB* db = manager->ref();
    int count = (int)est_db_size(db);
    manager->deref();
    return count;
}
int64_t
EstraierIndexReader::getDocumentId(const string& uri) {
    ESTDB* db = manager->ref();
    int64_t id = est_db_uri_to_id(db, uri.c_str());
    manager->deref();
    return id;
}
time_t
EstraierIndexReader::getMTime(int64_t docid) {
    ESTDB* db = manager->ref();
    time_t mtime = -1;
    char *cstr = est_db_get_doc_attr(db, docid, "@mdate");
    if (cstr) {
        mtime = atoi(cstr);
        free(cstr);
    }
    manager->deref();
    return mtime;

}
vector<string>
EstraierIndexReader::getFieldNames() {
    return vector<string>();
}
vector<pair<string,uint32_t> >
EstraierIndexReader::getHistogram( const string& query, const string& fieldname,
            const string& labeltype) {
    return vector<pair<string,uint32_t> >();
}

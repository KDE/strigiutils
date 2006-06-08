#include "estraierindexreader.h"
#include "estraierindexmanager.h"
#include <estraier.h>
#include <set>
#include <sstream>
#include <assert.h>
using namespace std;
using namespace jstreams;

EstraierIndexReader::EstraierIndexReader(EstraierIndexManager* m, ESTDB* d)
    : manager(m), db(d) {
}
EstraierIndexReader::~EstraierIndexReader() {
}
ESTCOND*
EstraierIndexReader::createCondition(const jstreams::Query& query) {
    // build the phrase string
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
    if (phrase.length() == 0) {
        phrase = "*";
    }
    terms.clear();
    const map<string, set<string> >& excludes = query.getExcludes();
    i = excludes.find("");
    if (i != excludes.end()) terms = i->second;
    for (j = terms.begin(); j != terms.end(); ++j) {
        phrase += " ANDNOT " + *j;
    }
    ESTCOND* cond = est_cond_new();
    printf("%s", phrase.c_str());
    est_cond_set_phrase(cond, phrase.c_str());
    est_cond_set_options(cond, ESTCONDSCFB);

    // add the attributes
    for (i = includes.begin(); i != includes.end(); ++i) {
        if (i->first.length() == 0) continue;
        string id = mapId(i->first);
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            string att = id + " STRINC " + *j;
            printf(" && %s", att.c_str());
            est_cond_add_attr(cond, att.c_str());
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
EstraierIndexReader::mapId(const std::string& id) {
    if (id == "path") return "@uri";
    if (id == "mtime") return "@mdata";
    if (id == "title") return "@title";
    if (id == "size") return "@size";
    return id.c_str();
}
string
EstraierIndexReader::getFragment(ESTDOC* doc, const Query& query) {
    char* f = est_doc_cat_texts(doc);
    string fragment = f;
//    printf("%s\n", f);
    free(f);
    return fragment;
}
int
EstraierIndexReader::countHits(const jstreams::Query& query) {
    ESTCOND* cond = createCondition(query);
    //est_cond_set_max(cond, 10);
    int n;
    int* ids;

    manager->ref();
    ids = est_db_search(db, cond, &n, NULL);
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

    manager->ref();
    ids = est_db_search(db, cond, &n, NULL);
    
    std::vector<IndexedDocument> results;
    for (int i=0; i<n; ++i) {
        int id = ids[i];
        char* uri = est_db_get_doc_attr(db, id, "@uri");
        if (uri) {
            IndexedDocument doc;
            doc.filepath = uri;
            doc.score = est_cond_score(cond, i);
            ESTDOC* d = est_db_get_doc(db, id, ESTGDNOKWD);
            doc.fragment = getFragment(d, query);
            CBLIST* atts = est_doc_attr_names(d);
            for (int j = 0; j < cblistnum(atts); ++j) {
                const char* name = cblistval(atts, j, 0);
                const char* value = est_doc_attr(d, name);
                if (*name == '@') name++;
                if (strcmp(name, "uri") != 0) {
                    doc.properties[name] = value;
                }
            }
            cblistclose(atts);
            results.push_back(doc);
            free(uri);
        }
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

    manager->ref();
    ids = est_db_search(db, cond, &n, NULL);
    
    for (int i=0; i<n; ++i) {
        char* uri = est_db_get_doc_attr(db, ids[i], "@uri");
        char* mdate = est_db_get_doc_attr(db, ids[i], "@mdate");
        time_t md = atoi(mdate);
        assert(uri);
        files[uri] = md;
        free(uri);
    }
    manager->deref();

    // clean up
    est_cond_delete(cond);
    free(ids);
    return files;
}
int
EstraierIndexReader::countDocuments() {
    manager->ref();
    int count = est_db_doc_num(db);
    manager->deref();
    return count;
}
int
EstraierIndexReader::countWords() {
    manager->ref();
    int count = est_db_word_num(db);
    manager->deref();
    return count;
}
int
EstraierIndexReader::getIndexSize() {
    manager->ref();
    int count = (int)est_db_size(db);
    manager->deref();
    return count;
}

#include "indexreader.h"
using namespace jstreams;
using namespace std;

/**
 * The constructor parses the query in include and exclude statements.
 * The following lines contain example queries.
 * hi
 * 'hi'
 * hi Jos
 * 'hi Jos'
 * "hi Jos"
 * -hi Jos
 * path:"hi Jos"
 * -path:"hi Jos"
 * So the syntax is something like this:
 * query ::= [term]*
 * term ::= [-][prefix]:("searchphrase"|searchphrase)
 **/

Query::Query(const string& query) {
    const char* q = query.c_str();
    const char* end = q+query.length();
    const char* p = q;
    while (p < end) {
        p = parseTerm(p);
    }
/*    map<string, set<string> >::const_iterator i;
    set<string>::const_iterator j;
    for (i = includes.begin(); i != includes.end(); ++i) {
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            printf("+ '%s':'%s'\n", i->first.c_str(), j->c_str());
        }
    }
    for (i = excludes.begin(); i != excludes.end(); ++i) {
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            printf("- '%s':'%s'\n", i->first.c_str(), j->c_str());
        }
    }
    printf("--\n");*/
}
const char*
Query::parseTerm(const char* s) {
    bool include = true;
    const char* p = s;
    // skip whitespace
    while (*p != '\0' && isspace(*p)) p++;
    if (*p == '\0') return p;

    // check for a - sign
    if (*p == '-') {
        include = false;
        p++;
    }
    // skip whitespace
    while (*p != '\0' && isspace(*p)) p++;
    if (*p == '\0') return p;

    char quote = 0;
    if (*p == '\'' || *p == '"') {
        quote = *p++;
        if (*p == '\0') return p;
    }
    const char* prefix = 0;
    const char* prefend;
    const char* term = p;
    // skip until end of string or closing quote or colon or whitespace
    while (*p != '\0' && ((quote == 0 && !isspace(*p))
            || (quote != 0 && *p != quote))) {
        if (quote == 0 && *p == ':') {
            // define the prefix
            prefix = term;
            prefend = p;
            ++p;
            if (*p == '\0') return p;
            if (*p == '\'' || *p == '"') {
                quote = *p++;
                if (*p == '\0') return p;
            }
            term = p;
        }
        ++p;
    }
    if (*term == '\0') return term;
    if (p - term > 0) {
        string pre;
        string ter;
        if (prefix != 0 && term - prefix > 1) {
            pre = string(prefix, prefend-prefix);
        }
        ter = string(term, p-term);
        if (include) {
            includes[pre].insert(ter);
        } else {
            excludes[pre].insert(ter);
        }
    }
    // skip the terminating character
    if (p != '\0') p++;
    return p;
}

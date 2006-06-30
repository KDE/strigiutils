#include "indexreader.h"
using namespace std;
using namespace jstreams;

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

Query::Query(const string& query, int max, int offset) {
    this->max = max;
    this->offset = offset;
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

void
replaceall(string& text, const string& a, const string& b) {
    size_t pos = 0;
    pos = text.find(a);
    while (pos != string::npos) {
        text.replace(pos, a.length(), "&lt;");
        pos = text.find('<');
    }
}
string
Query::highlight(const string& text) const {
    return text;
    int pre = 5, post = 5, maxlen = 100;
    string t = text;
    replaceall(t, "<", "&lt;");
    replaceall(t, ">", "&gt;");
    string lt = t;
    for (uint i=0; i<t.length(); ++i) {
        lt[i] = tolower(lt[i]);
    }
    vector<string> re;
    map<string, set<string> >::const_iterator i;
    for (i = includes.begin(); i != includes.end(); ++i) {
        set<string>::const_iterator j;
        for (j = i->second.begin(); j != i->second.end(); ++j) {
            string s = *j;
            for (int k = 0; k<s.length(); ++k) {
                s[k] = tolower(s[k]);
            }
            re.push_back(s);
        }
    }
    string out;
    int pos = 0;
    int last1 = string::npos;
    int last2 = string::npos;
    int last3 = string::npos;
    int last4 = string::npos;
    vector<string>::const_iterator k;
    while (pos >= 0 && out.length()+last1-last4 < maxlen) {
        int rep = string::npos;
        int len;
        for (k = re.begin(); k != re.end(); ++k) {
            int p = lt.find(*k, pos);
            if (p > 0 && (rep == string::npos || p < rep)) {
                rep = p;
                len = k->length();
            }
        }
        if (rep >= 0) {
            int p1 = t.find(" ", rep-pre);
            if (p1 == string::npos) p1 = (rep-pre < 0) ?0 : rep-pre;
            int p4 = t.find(" ", rep+len+post);
            if (p4 == string::npos) p4 = t.length();
            out += t.substr(p1, rep-p1);
            out += "<b>";
            out += t.substr(rep, len);
            out += "</b>";
            out += t.substr(rep+len, p4-(rep+len));
            out += " ... ";
            /* if (lasts == string::npos) {
                lasts = s;
            } else if (s > laste) {
                if (out.length() == 0 && lasts > 0) out += "... ";
                out += t.substr(lasts, laste - lasts) + " ... ";
                lasts = s;
            }
            laste = e;*/
            pos = rep+1;
        } else {
            pos = rep;
        }
    }
   /* if (lasts != string::npos) {
        if (out.length() == 0 && lasts > 0) out += "... ";
        out += t.substr(lasts, laste - lasts) + " ... ";
    }
    for (k = re.begin(); k != re.end(); ++k) {
        replaceall(out, *k, "<b>great</b>");
    }*/
    if (out.length() == 0) {
        out = t.substr(0, 100);
    }
    return out;
}

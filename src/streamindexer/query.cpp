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
#include "query.h"
#include "jstreamsconfig.h"
#include "indexreader.h"
using namespace std;
using namespace jstreams;

class Query::Term {
public:
    string prefix;
    string term;
    bool include;

    void clear() {
        prefix = term = "";
    }
};

Query::Query() {
}
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
    Term term;
    Term lastterm;
    bool hador = false;
    while (p < end) {
        term.clear();
        p = parseTerm(p, term);
        if (term.term == "OR") {
            hador = true;
            Query q;
            q.addTerm(lastterm);
            alternatives.insert(q);
        } else {
            addTerm(lastterm);
            lastterm = term;
        }
    }
    addTerm(lastterm);
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
}/*
bool
operator<(const jstreams::Query&a,const jstreams::Query&b) {
    return &a < &b;
}*/
void
Query::addTerm(const Term& term) {
    if (term.term.size() == 0) return;
    if (term.include) {
        includes[term.prefix].insert(term.term);
    } else {
        excludes[term.prefix].insert(term.term);
    }
}
const char*
Query::parseTerm(const char* s, Term& parsedterm) {
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
        parsedterm.include = include;
        if (prefix != 0 && term - prefix > 1) {
            parsedterm.prefix = string(prefix, prefend-prefix);
        }
        parsedterm.term = string(term, p-term);
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
            for (uint k = 0; k<s.length(); ++k) {
                s[k] = tolower(s[k]);
            }
            re.push_back(s);
        }
    }
    string out;
    int pos = 0;
    int last1 = string::npos;
    //int last2 = string::npos;
    //int last3 = string::npos;
    int last4 = string::npos;
    vector<string>::const_iterator k;
    while (pos >= 0 && (int)(out.length()+last1-last4) < maxlen) {
        uint rep = string::npos;
        int len;
        for (k = re.begin(); k != re.end(); ++k) {
            uint p = lt.find(*k, pos);
            if (p > 0 && (rep == string::npos || p < rep)) {
                rep = p;
                len = k->length();
            }
        }
        if (rep >= 0) {
            uint p1 = t.find(" ", rep-pre);
            if (p1 == string::npos) p1 = (rep-pre < 0) ?0 : rep-pre;
            uint p4 = t.find(" ", rep+len+post);
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

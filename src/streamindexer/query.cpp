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
#include "jstreamsconfig.h"
#include "indexreader.h"
#include <ctype.h>
using namespace std;
using namespace jstreams;

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
Query::Query(int max, int offset) {
    this->max = max;
    this->offset = offset;
}
QueryParser::QueryParser() {
    defaultFields.push_back("content");
    defaultFields.push_back("artist");
    defaultFields.push_back("filename");
    defaultFields.push_back("album");
    defaultFields.push_back("title");
}
Query
QueryParser::buildQuery(const string& querystring, int32_t max, int32_t offset){
    const char* q = querystring.c_str();
    const char* end = q + querystring.length();
    const char* p = q;
    Query query(max, offset);
    Query term;
    Query lastterm;
    bool hadOr = false;
    while (p < end) {
        term.clear();
        p = parseQuery(p, term);
        if (term.expression == "OR") {
            hadOr = true;
            Query q;
            addQuery(q, lastterm);
        } else {
            addQuery(query, lastterm);
            lastterm = term;
        }
    }
    addQuery(query, lastterm);
    return query;
}/*
bool
operator<(const Query&a,const Query&b) {
    return &a < &b;
}*/
void
QueryParser::addQuery(Query& query, const Query& subquery) const {
    // if the subquery is empty, do not add it
    if (subquery.terms.size() == 0 && subquery.expression.size() == 0) return;
    // if the subquery has no field name, use the default field names
    if (subquery.expression.size() > 0 && subquery.fieldname.size() == 0
            && defaultFields.size() > 0) {
        if (defaultFields.size() == 1) {
            Query sq = subquery;
            sq.fieldname = *defaultFields.begin();
            query.terms.push_back(sq);
        } else {
            list<string>::const_iterator i;
            Query orQuery;
            orQuery.occurrence = subquery.occurrence;
            for (i = defaultFields.begin(); i != defaultFields.end(); ++i) {
                Query sub;
                sub.fieldname = *i;
                sub.expression = subquery.expression;
                sub.occurrence = Query::SHOULD;
                orQuery.terms.push_back(sub);
             }
             query.terms.push_back(orQuery);
        }
    } else {
        query.terms.push_back(subquery);
    }
}
const char*
QueryParser::parseQuery(const char* s, Query& parsedterm) const {
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
    const char* prefend = 0;
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
        parsedterm.occurrence = (include) ?Query::MUST :Query::MUST_NOT;
        if (prefix != 0 && term - prefix > 1) {
            parsedterm.fieldname = string(prefix, prefend-prefix);
        }
        parsedterm.expression = string(term, p-term);
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
void
Query::clear() {
    terms.clear();
    occurrence = MUST;
    fieldname = expression = "";
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
    list<Query>::const_iterator i;
    for (i = terms.begin(); i != terms.end(); ++i) {
        if (i->occurrence != MUST_NOT) {
            string s = i->expression;
            for (uint k = 0; k < s.length(); ++k) {
                s[k] = tolower(s[k]);
            }
            re.push_back(s);
        }
    }
    string out;
    int pos = 0;
    string::size_type last1 = string::npos;
    //int last2 = string::npos;
    //int last3 = string::npos;
    string::size_type last4 = string::npos;
    vector<string>::const_iterator k;
    while (pos >= 0 && (int)(out.length()+last1-last4) < maxlen) {
        string::size_type rep = string::npos;
        int len;
        for (k = re.begin(); k != re.end(); ++k) {
            uint p = lt.find(*k, pos);
            if (p > 0 && (rep == string::npos || p < rep)) {
                rep = p;
                len = k->length();
            }
        }
        if (rep != string::npos) {
            string::size_type p1 = t.find(" ", rep-pre);
            if (p1 == string::npos) p1 = rep-pre;
            string::size_type p4 = t.find(" ", rep+len+post);
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

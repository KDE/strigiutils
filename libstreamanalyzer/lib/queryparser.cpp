/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include <strigi/queryparser.h>
#include <strigi/fieldpropertiesdb.h>
#include <iostream>
#include <cstring>

#include "xesamparser.h"

using namespace std;
using namespace Strigi;

void 
setModifier(char c, Query& q) {
    switch (c) {
    case 'c': q.term().setCaseSensitive(true);       break;
    case 'C': q.term().setCaseSensitive(false);      break;
    case 'd': q.term().setDiacriticSensitive(true);  break;
    case 'D': q.term().setDiacriticSensitive(false); break;
    case 'l': q.term().setStemming(false);           break;
    case 'L': q.term().setStemming(true);            break;
    case 'e': q.term().setCaseSensitive(true);
              q.term().setDiacriticSensitive(true);
              q.term().setStemming(false);           break;
    case 'f': q.term().setFuzzy(0.5);                break; // TODO: sane value
    case 'b': q.setBoost(2);                         break; // TODO: sane value
    case 'p': q.setType(Query::Proximity);
              q.term().setProximityDistance(10);     break;
    case 's': q.term().setSlack(1);                  break; // TODO: sane value
    case 'w': break; // TODO
    case 'o': q.term().setOrdered(true);             break;
    case 'r': q.setType(Query::RegExp);              break;
    }
}
void
setModifiers(const char*p, const char* pend, Query& q) {
    while (p < pend) {
        setModifier(*p, q);
        ++p;
    }
}

const char*
parse(const char* p, Query& q) {
    // set defaults
    q.setType(Query::Contains);

    while (*p && isspace(*p)) p++;

    // check if we must include or exclude
    if (*p == '+') {
        p++;
    } else if (*p == '-') {
        q.setNegate(true);
        p++;
    }
 
    // find the starts of the different parts
    const char* rel = p + strcspn(p, "=:<>#");
    const char* quote = p + strcspn(p, "'\"");
    const char* space = p + strcspn(p, "\t \r\n");

    // do we have a field name?
    if (*rel && rel < space && rel < quote) {
        q.fields().push_back(string(p, rel));
        p = rel+1;
        if (*rel == '=') {
            q.setType(Query::Equals);
        } else if (*rel == '#') {
	    q.setType(Query::Keyword);
	} else if (*rel == '<') {
            if (*p == '=') {
                p++;
                q.setType(Query::LessThanEquals);
            } else {
                q.setType(Query::LessThan);
            }
        } else if (*rel == '>') {
            if (*p == '=') {
                p++;
                q.setType(Query::GreaterThanEquals);
            } else {
                q.setType(Query::GreaterThan);
            }
        }
    }

    if (*quote && *(quote+1) && quote < space) {
	const char* pairquote = strchr(quote+1, *quote);
	if (pairquote != NULL) {
	    pairquote++;
	    // quoted phrase, could be with spaces
	    q.term().setValue(string(quote+1, pairquote-1));
	    if (*space) {
		if (space < pairquote) {
		    // everything between pairquote and next space is a modifier
		    space = pairquote + strcspn(pairquote, "\t \r\n");
		}
		if (space - pairquote + 1 > 0) {
		    // avoid empty modifiers
		    setModifiers(pairquote+1, space, q);
		}
	    }
	}
    } else {
	q.term().setValue(string(p, space));
    }
    return space+1;
}
void
prependXesamNamespace(Query& query) {
    // prepend the field names with the xesam namespace
    // this will be elaborated once the xesam spec continues
    vector<string>::iterator end(query.fields().end());
    FieldPropertiesDb& db = FieldPropertiesDb::db();
    for (vector<string>::iterator i = query.fields().begin(); i != end; ++i) {
        *i = db.propertiesByAlias(*i).uri();
    }
    std::vector<Query>::iterator qend(query.subQueries().end());
    for (vector<Query>::iterator i = query.subQueries().begin(); i!=qend; ++i) {
        prependXesamNamespace(*i);
    }
}
string
removeXML(const std::string& q) {
    string::size_type tagstart = q.find("<userQuery");
    string::size_type tagend = -1;
    string::size_type queryend = q.size();
    if (tagstart != string::npos) {
        // the query is enclosed in a tag
        tagend = q.find(">", tagstart);
        if (tagend != string::npos) {
            queryend = q.find("<", tagend);
            if (queryend == string::npos) {
                queryend = q.size();
            }
        } else {
            tagend = -1;
        }
    }
    while (tagend+1 < q.size() && isspace(q[tagend+1])) tagend++;
    while (queryend-1 > tagend && isspace(q[queryend-1])) queryend--;
    return q.substr(tagend+1, queryend-tagend-1);
}
Query
QueryParser::buildQuery(const std::string& xmlq) {

    Query query;

    string::size_type tagstart = xmlq.find("<query");
    if (tagstart != string::npos) { // xesam language query
        XesamParser parser;
        parser.buildQuery(xmlq, query);
    } else { // user language query
        const string q(removeXML(xmlq));

        query.setType(Query::And);
        query.subQueries().clear();

        Query sub;
        const char* p = q.c_str();
        const char* pend = p + q.length();
        while (p < pend) {
            p = ::parse(p, sub);
            query.subQueries().push_back(sub);
            sub = Query();
        }
        // normalize
        if (query.subQueries().size() == 1) {
            Query q = query.subQueries()[0];
            query = q;
        }
    }
    prependXesamNamespace(query);

    return query;
}

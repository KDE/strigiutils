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
#include "queryparser.h"
#include <iostream>
#include <cstring>
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
    case 'e': q.term().setCaseSensitive(true);       break;
              q.term().setDiacriticSensitive(true);  break;
              q.term().setStemming(false);           break;
    case 'f': q.term().setFuzzy(0.5);                break; // TODO: sane value
    case 'b': q.setBoost(2);                         break; // TODO: sane value
    case 'p': q.setType(Query::Proximity);
              q.term().setProximityDistance(10);     break;
    case 's': q.term().setSlack(1);                  break; // TODO: sane value
    case 'w': break; // TODO
    case 'o': q.term().setOrdered(true);
    case 'r': q.setType(Query::RegExp);
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

    // check if we must include or exclude
    if (*p == '+') {
        p++;
    } else if (*p == '-') {
        q.setNegate(true);
        p++;
    }
 
    // find the starts of the different parts
    const char* rel = p + strcspn(p, "=:<>");
    const char* quote = p + strcspn(p, "'\"");
    const char* space = p + strcspn(p, "\t \r\n");

    // do we have a field name?
    if (rel < quote && rel < space) {
        q.fields().push_back(string(p, rel));
        p = rel+1;
        if (*rel == '=') {
            q.setType(Query::Equals);
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
    // do we have a phrase or a word?
    if (quote < space) { // phrase
        // find closing quote
        const char* end = strchr(quote+1, *quote);
        if (end) {
            q.term().setValue(string(quote+1, end-1));
        } else { // invalid: no closing quote
            return space;
        }
        // find the first space after the closing quote
        space = end + strcspn(end, "\t \r\n");
        setModifiers(end+1, space, q); 
    } else {
        q.term().setValue(string(p, space));
    }
    return space+1;
}

Query
QueryParser::buildQuery(const std::string& q) {
    Query query;
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
    return query;
}

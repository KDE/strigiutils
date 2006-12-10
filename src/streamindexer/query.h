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

#include <map>
#include <string>
#include <set>
#include <list>

namespace jstreams {
/**
 * Break up a string in a query.
 * Currently very simple. Currently always combines terms with AND.
 **/
class Query {
public:
    enum Occur { MUST, MUST_NOT, SHOULD };
private:
    std::string fieldname;
    std::string expression;
    Occur occurance;
    std::list<Query> terms;
    int max;
    int offset;

    void addQuery(const Query& term);
    const char* parseQuery(const char*, Query& term);
    void clear();
public:
    Query();
    Query(const std::string& q, int max, int offset);
    int getMax() const { return max; }
    int getOffset() const { return offset; }
    const std::string getExpression() const { return expression; }
    const std::string getFieldName() const { return fieldname; }
    const std::list<Query>& getTerms() const { return terms; }
    Occur getOccurance() const { return occurance; }
    std::string highlight(const std::string& text) const;
};

class QueryParser {
private:
    std::list<std::string> defaultFields;
public:
    QueryParser();
    void setDefaultFields(const std::list<std::string>& df) {
        defaultFields = df;
    }
    const std::list<std::string>& getDefaultFields() const {
        return defaultFields;
    }
    Query buildQuery(const std::string& a);
};

}
bool operator<(const jstreams::Query&,const jstreams::Query&);


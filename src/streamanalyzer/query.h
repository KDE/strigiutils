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

#include "jstreamsconfig.h"

namespace Strigi {
/**
 * Break up a string in a query.
 * Currently very simple. Currently always combines terms with AND.
 **/
class STREAMANALYZER_EXPORT Query {
friend class QueryParser;
public:
    enum Occurrence { MUST, MUST_NOT, SHOULD };
private:
    std::string fieldname;
    std::string expression;
    Occurrence occurrence;
    std::list<Query> terms;
    int max;
    int offset;

    void clear();
    Query(int max, int offset);
public:
    Query();
    int getMax() const { return max; }
    int getOffset() const { return offset; }
    const std::string getExpression() const { return expression; }
    const std::string getFieldName() const { return fieldname; }
    const std::list<Query>& getTerms() const { return terms; }
    Occurrence getOccurrence() const { return occurrence; }
    std::string highlight(const std::string& text) const;
};

class STREAMANALYZER_EXPORT QueryParser {
private:
    std::list<std::string> defaultFields;
    const char* parseQuery(const char*, Query& term) const;
    void addQuery(Query& query, const Query& subquery) const;
public:
    QueryParser();
    void setDefaultFields(const std::list<std::string>& df) {
        defaultFields = df;
    }
    const std::list<std::string>& getDefaultFields() const {
        return defaultFields;
    }
    Query buildQuery(const std::string& a, int32_t max, int32_t offset);
};

}
bool operator<(const Strigi::Query&,const Strigi::Query&);


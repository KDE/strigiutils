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
    std::string m_fieldname;
    std::string m_expression;
    Occurrence m_occurrence;
    std::list<Query> m_terms;
    int m_max;
    int m_offset;

    void clear();
    Query(int max, int offset);
public:
    Query();
    int max() const { return m_max; }
    int offset() const { return m_offset; }
    const std::string expression() const { return m_expression; }
    const std::string fieldName() const { return m_fieldname; }
    const std::list<Query>& terms() const { return m_terms; }
    Occurrence occurrence() const { return m_occurrence; }
    std::string highlight(const std::string& text) const;
};

class STREAMANALYZER_EXPORT QueryParser {
private:
    std::list<std::string> m_defaultFields;
    const char* parseQuery(const char*, Query& term) const;
    void addQuery(Query& query, const Query& subquery) const;
public:
    QueryParser();
    void setDefaultFields(const std::list<std::string>& df) {
        m_defaultFields = df;
    }
    const std::list<std::string>& defaultFields() const {
        return m_defaultFields;
    }
    Query buildQuery(const std::string& a, int32_t max, int32_t offset);
};

}
bool operator<(const Strigi::Query&,const Strigi::Query&);


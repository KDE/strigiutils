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
#ifndef STRIGI_QUERY_H
#define STRIGI_QUERY_H

#include <strigi/strigiconfig.h>
#include <ostream>
#include <vector>
#include <string>

namespace Strigi {

/** these classes capture the query capabilities of Xesam **/
class TermPrivate;
class STREAMANALYZER_EXPORT Term {
private:
    TermPrivate* const p;
public:
    // FIXME (trueg): there is not method to retrieve the type
    enum Type { String, Integer, Date, Boolean, Float };

    Term();
    Term(const Term&);
    ~Term();
    void operator=(const Term&);

    bool caseSensitive() const;
    void setCaseSensitive(bool b);
    bool diacriticSensitive() const;
    void setDiacriticSensitive(bool b);
    bool stemming() const;
    void setStemming(bool b);
    float fuzzy() const;
    void setFuzzy(float f);
    int slack() const;
    void setSlack(int s);
    int proximityDistance() const;
    void setProximityDistance(int d);
    bool ordered() const;
    void setOrdered(bool b);
    bool wordBased() const;
    void setWordBased(bool b);

    void setValue(const std::string& s);
    void setValue(const char* s);

    const std::string& string() const;
};

class QueryPrivate;
class STREAMANALYZER_EXPORT Query {
private:
    QueryPrivate* const p;
public:
    // FIXME (trueg): why is NOT not an operator but an extra method?
    //                it makes recursive handling of the query less clean
    enum Type { And, Or,
        Equals, Contains, LessThan, LessThanEquals,
        GreaterThan, GreaterThanEquals, StartsWith, FullText, RegExp,
        Proximity, Keyword };

    Query();
    Query(const Query& q);
    ~Query();
    void operator=(const Query&);

    Term& term();
    const Term& term() const;
    void setTerm(const Term& t);
    float boost() const;
    void setBoost(float i);
    Type type() const;
    void setType(Type t);
    bool negate() const;
    void setNegate(bool n);
    std::vector<std::string>& fields();
    const std::vector<std::string>& fields() const;
    std::vector<Query>& subQueries();
    const std::vector<Query>& subQueries() const;

    /**
     * determine if the query is valid or not.
     * A valid query may not contain invalid subqueries.
     **/
    bool valid() const;
};
}

STREAMANALYZER_EXPORT std::ostream &operator<<(std::ostream &stream, Strigi::Term term);
STREAMANALYZER_EXPORT std::ostream &operator<<(std::ostream &stream, Strigi::Query query);
#endif

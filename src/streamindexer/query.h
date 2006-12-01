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

namespace jstreams {
class Query;
/**
 * Break up a string in a query.
 * Currently very simple. Currently always combines terms with AND.
 **/
class Query {
private:
    class Term;
    int max;
    int offset;
    std::map<std::string, std::set<std::string> > includes;
    std::map<std::string, std::set<std::string> > excludes;

    void addTerm(const Term& term);
    const char* parseTerm(const char*, Term& term);
public:
    Query();
    Query(const std::string& q, int max, int offset);
    const std::map<std::string, std::set<std::string> > &getIncludes() const {
        return includes;
    }
    const std::map<std::string, std::set<std::string> > &getExcludes() const {
        return excludes;
    }
    int getMax() const { return max; }
    int getOffset() const { return offset; }
    std::string highlight(const std::string& text) const;
};
}
bool operator<(const jstreams::Query&,const jstreams::Query&);


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
#include "query.h"
using namespace std;
using namespace Strigi;

class Strigi::TermPrivate {
public:
    int64_t i_value;     // value for Integer, Date and Boolean
    double d_value;      // value for Float
    std::string s_value; // value for String

    // flags for String terms (ordered by size)
    std::string language;
    float fuzzy;
    int slack;
    int proximityDistance;
    Term::Type type;
    bool phrase;
    bool caseSensitive;
    bool diacriticSensitive;
    bool ordered;
    bool enableStemming;
    bool wordbased;
};
class Strigi::QueryPrivate {
public:
    // Selection fields
    Term term;
    Term termTwo; // needed for Proximity
    std::vector<std::string> fields;
    // Collectible fields
    std::vector<Query> subs;
    // Shared fields
    float boost;
    bool negate;
    // Type
    Query::Type type;

    QueryPrivate() :boost(1), negate(false), type(Query::Contains) {}
};

Term::Term() :p(new TermPrivate()) {
    // FIXME (trueg): The term is completely uninitialized here. The values as well as the type are random.
    //                Create a TermPrivate constructor that initializes all to 0 and sets the type to s.th. like "None" or "Empty"
}
Term::Term(const Term& t) :p(new TermPrivate(*t.p)) {
}
Term::~Term() {
    delete p;
}
void
Term::operator=(const Term& t) {
    *p = *t.p;
}
int
Term::proximityDistance() const {
    return p->proximityDistance;
}
void
Term::setProximityDistance(int d) {
    p->proximityDistance = d;
}
bool
Term::caseSensitive() const {
    return p->caseSensitive;
}
void
Term::setCaseSensitive(bool b) {
    p->caseSensitive = b;
}
bool
Term::diacriticSensitive() const {
    return p->diacriticSensitive;
}
void
Term::setDiacriticSensitive(bool b) {
    p->diacriticSensitive = b;
}
bool
Term::stemming() const {
    return p->enableStemming;
}
void
Term::setStemming(bool b) {
    p->enableStemming = b;
}
float
Term::fuzzy() const {
    return p->fuzzy;
}
void
Term::setFuzzy(float f) {
    p->fuzzy = f;
}
int
Term::slack() const {
    return p->slack;
}
void
Term::setSlack(int s) {
    p->slack = s;
}
bool
Term::ordered() const {
    return p->ordered;
}
void
Term::setOrdered(bool b) {
    p->ordered = b;
}
bool
Term::wordBased() const {
    return p->wordbased;
}
void
Term::setWordBased(bool b) {
    p->wordbased = b;
}
void
Term::setValue(const std::string& s) {
    p->s_value = s;
    p->type = String;
}
void
Term::setValue(const char* s) {
    p->s_value.assign(s);
    p->type = String;
}
const string&
Term::string() const {
    return p->s_value;
}
Query::Query() :p(new QueryPrivate()) {
}
Query::Query(const Query& t) :p(new QueryPrivate(*t.p)) {
}
Query::~Query() {
    delete p;
}
void
Query::operator=(const Query& q) {
    *p = *q.p;
}
Term&
Query::term() {
    return p->term;
}
const Term&
Query::term() const {
    return p->term;
}
float
Query::boost() const {
    return p->boost;
}
void
Query::setBoost(float b) {
    p->boost = b;
}
Query::Type
Query::type() const {
    return p->type;
}
void
Query::setType(Type t) {
    p->type = t;
}
bool
Query::negate() const {
    return p->negate;
}
void
Query::setNegate(bool n) {
    p->negate = n;
}
const vector<string>&
Query::fields() const {
    return p->fields;
}
vector<string>&
Query::fields() {
    return p->fields;
}
const vector<Query>&
Query::subQueries() const {
    return p->subs;
}
vector<Query>&
Query::subQueries() {
    return p->subs;
}

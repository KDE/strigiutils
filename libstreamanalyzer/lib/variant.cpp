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
#include <strigi/variant.h>

using namespace std;
using namespace Strigi;

class Strigi::VariantPrivate {
public:
    int32_t i_value;
    uint32_t u_value;
    string s_value;
    vector<string> as_value;
    vector<vector<string> > aas_value;
    Variant::Type vartype;
    bool valid;

    VariantPrivate() :i_value(0), u_value(0), vartype(Variant::b_val), valid(false) {}
    VariantPrivate(const Variant& v) { *this = v; }
    void operator=(const VariantPrivate& v);
    bool b() const;
    int32_t i() const;
    uint32_t u() const;
    string s() const;
    vector<string> as() const;
    vector<vector<string> > aas() const;

    static string itos(int32_t i);
};
Variant::Variant() :p(new VariantPrivate()) {}
Variant::Variant(bool v) :p(new VariantPrivate()) {
    *this=v;
}
Variant::Variant(int32_t v) :p(new VariantPrivate()) {
    *this=v;
}
Variant::Variant(uint32_t v) :p(new VariantPrivate()) {
    *this=v;
}
Variant::Variant(const char* v) :p(new VariantPrivate()) {
    *this=v;
}
Variant::Variant(const string& v) :p(new VariantPrivate()) {
    *this=v;
}
Variant::Variant(const vector<string>& v) :p(new VariantPrivate()) {
    *this=v;
}
Variant::Variant(const vector<vector<string> >& v) :p(new VariantPrivate()) {
    *this=v;
}
Variant::Variant(const Variant& v) :p(new VariantPrivate(*v.p)) {
}
Variant::~Variant() {
  delete p;
}

Variant::Type
Variant::type() const { return p->vartype; }
const Variant&
Variant::operator=(bool v) {
    p->valid = true;
    p->i_value = v;
    p->vartype = b_val;
    return *this;
}
const Variant&
Variant::operator=(int32_t v) {
    p->valid=true;
    p->i_value = v;
    p->u_value = v;
    p->vartype = i_val;
    return *this;
}
const Variant&
Variant::operator=(uint32_t v) {
    p->valid=true;
    p->i_value = v;
    p->u_value = v;
    p->vartype = u_val;
    return *this;
}
const Variant&
Variant::operator=(const char* v) {
    p->valid=true;
    p->s_value.assign(v);
    p->vartype = s_val;
    return *this;
}
const Variant&
Variant::operator=(const string& v) {
    p->valid=true;
    p->s_value.assign(v);
    p->vartype = s_val;
    return *this;
}
const Variant&
Variant::operator=(const vector<string>& v) {
    p->valid=true;
    p->as_value = v;
    p->vartype = as_val;
    return *this;
}
const Variant&
Variant::operator=(const vector<vector<string> >& v) {
    p->valid=true;
    p->aas_value = v;
    p->vartype = aas_val;
    return *this;
}
const Variant&
Variant::operator=(const Variant& v) {
    *p = *v.p;
    return v;
}
void
VariantPrivate::operator=(const VariantPrivate& v) {
    i_value = v.i_value;
    s_value = v.s_value;
    as_value = v.as_value;
    vartype = v.vartype;
    valid = v.valid;
}
bool
Variant::b() const {
    return p->b();
}
bool
VariantPrivate::b() const {
    switch (vartype) {
    case Variant::b_val:
    case Variant::i_val:
         return i_value != 0;
    case Variant::s_val:
         return s_value == "1" || s_value == "true" || s_value == "True"
             || s_value == "TRUE";
    case Variant::as_val:
         return as_value.size() != 0;
    default:
         return false;
    }
}
int32_t
Variant::i() const {
    return p->i();
}
int32_t
VariantPrivate::i() const {
    switch (vartype) {
    case Variant::b_val:
    case Variant::i_val:
         return i_value;
    case Variant::s_val:
         return atoi(s_value.c_str());
    case Variant::as_val:
         return (int32_t)as_value.size();
    default:
         return -1;
    }
}
uint32_t
Variant::u() const {
    return p->u();
}
uint32_t
VariantPrivate::u() const {
    switch (vartype) {
    case Variant::b_val:
    case Variant::i_val:
    case Variant::u_val:
         return u_value;
    case Variant::s_val:
         return atoi(s_value.c_str());
    case Variant::as_val:
         return (int32_t)as_value.size();
    default:
         return ~0U;
    }
}
string
VariantPrivate::itos(int32_t i) {
    ostringstream o;
    o << i;
    return o.str();
}
string
Variant::s() const {
    return p->s();
}
string
VariantPrivate::s() const {
    switch (vartype) {
    case Variant::b_val:
         return i_value ?"true" :"false";
    case Variant::i_val:
         return itos(i_value);
    case Variant::s_val:
         return s_value;
    case Variant::as_val:
         return as_value.size() ?as_value[0] :"";
    default:
         return "";
    }
}
vector<string>
Variant::as() const {
    return p->as();
}
vector<string>
VariantPrivate::as() const {
    if (vartype == Variant::as_val) {
        return as_value;
    }
    vector<string> v;
    if (b()) {
        v.push_back(s());
    }
    return v;
}
vector<vector<string> >
Variant::aas() const {
    return p->aas();
}
vector<vector<string> >
VariantPrivate::aas() const {
    if (vartype == Variant::aas_val) {
        return aas_value;
    }
    vector<vector<string> > v;
    if (b()) {
        v.push_back(as());
    }
    return v;
}
bool
Variant::isValid() const {
    return p->valid;
}

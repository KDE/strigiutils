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
#include "fieldproperties.h"
#include "fieldpropertiesdb.h"
#include <map>
using namespace Strigi;
using namespace std;

const string FieldProperties::Private::empty;
FieldProperties::FieldProperties() :p(new Private()) {
}
FieldProperties::FieldProperties(const Private& pr) :p(new Private(pr)) {}
FieldProperties::FieldProperties(const string& k)
        :p(new Private(k)) {
    const FieldProperties* fp = FieldPropertiesDb::db().getProperties(k);
    if (fp) {
        *this = *fp;
    }
}
FieldProperties::FieldProperties(const FieldProperties& p)
    :p(new Private(*p.p)) {
}
FieldProperties::~FieldProperties() {
    delete p;
}
const FieldProperties&
FieldProperties::operator=(const FieldProperties& f) {
    *p = *f.p;
    return f;
}
const std::string&
FieldProperties::uri() const {
    return p->uri;
}
const std::string&
FieldProperties::name() const {
    return p->name;
}
const std::string&
FieldProperties::typeUri() const {
    return p->typeuri;
}
const std::string&
FieldProperties::description() const {
    return p->description;
}
const std::string&
FieldProperties::localizedName(const std::string& locale) const {
    map<string,pair<string,string> >::iterator i = p->localized.find(locale);
    return (i == p->localized.end()) ?Private::empty :i->second.first;
}
const std::string&
FieldProperties::localizedDescription(const std::string& locale) const {
    map<string,pair<string,string> >::iterator i = p->localized.find(locale);
    return (i == p->localized.end()) ?Private::empty :i->second.second;
}
const std::list<std::string>&
FieldProperties::parentUris() const {
    return p->parentUris;
}

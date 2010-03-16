/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Alexandr Goncearenco <neksa@neksa.net>
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
#include <strigi/classproperties.h>
#include <strigi/fieldpropertiesdb.h>
#include "fieldproperties_private.h"
#include <map>
using namespace Strigi;
using namespace std;

ClassProperties::ClassProperties() :p(new Private()) {
}
ClassProperties::ClassProperties(const Private& pr) :p(new Private(pr)) {}
ClassProperties::ClassProperties(const string& k)
        :p(new Private(k)) {
    const ClassProperties& fp = FieldPropertiesDb::db().classes(k);
    if (fp.valid()) {
        *this = fp;
    }
}
ClassProperties::ClassProperties(const ClassProperties& p)
    :p(new Private(*p.p)) {
}
ClassProperties::~ClassProperties() {
    delete p;
}
const ClassProperties&
ClassProperties::operator=(const ClassProperties& f) {
    *p = *f.p;
    return f;
}
bool
ClassProperties::valid() const {
    return p->uri.size() != 0;
}
const string&
ClassProperties::uri() const {
    return p->uri;
}
const string&
ClassProperties::name() const {
    return p->name;
}
const string&
ClassProperties::description() const {
    return p->description;
}
const std::vector<std::string>&
ClassProperties::locales() const {
    return p->locales;
}
const string&
ClassProperties::localizedName(const string& locale) const {
    map<string,Localized>::iterator i = p->localized.find(locale);
    return (i == p->localized.end()) ?empty() :i->second.name;
}
const string&
ClassProperties::localizedDescription(const string& locale) const {
    map<string,Localized>::iterator i = p->localized.find(locale);
    return (i == p->localized.end()) ?empty() :i->second.description;
}
const vector<string>&
ClassProperties::parentUris() const {
    return p->parentUris;
}
const vector<string>&
ClassProperties::childUris() const {
    return p->childUris;
}
const vector<string>&
ClassProperties::applicableProperties() const {
    return p->applicableProperties;
}

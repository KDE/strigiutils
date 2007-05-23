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
#include "xesamsession.h"
#include "xesamsearch.h"
#include "xesamlivesearch.h"
#include <cstdlib>
using namespace std;

XesamSession::XesamSession(XesamLiveSearch& x) :xesam(x),
    searchLive(false),
    searchBlocking(true),
    hitSnippetLength(200),
    sortPrimary("score"),
    sortAscending(false) {
}

Variant
XesamSession::setProperty(const std::string& prop, const Variant& v) {
    Variant o;
    if (prop == "search.live") {
         o = searchLive = v.b();
    } else if (prop == "search.blocking") {
         o = searchBlocking = v.b();
    } else if (prop == "hit.fields") {
         o = hitFields = v.as();
    } else if (prop == "hit.fields.extended") {
         o = hitFieldsExtended = v.as();
    } else if (prop == "hit.snippet.length") {
         o = hitSnippetLength = max(v.i(), 0);
    } else if (prop == "sort.primary") {
         o = sortPrimary = v.s();
    } else if (prop == "sort.secondary") {
         o = sortSecondary = v.s();
    } else if (prop == "sort.order") {
         sortAscending = v.s() == "ascending";
         o = sortAscending ?"ascending" :"descending";
    } else if (prop == "vendor.id") {
         o = getProperty(prop);
    }
    return o;
}
Variant
XesamSession::getProperty(const std::string& prop) {
    Variant o;
    if (prop == "search.live") {
         o = searchLive;
    } else if (prop == "search.blocking") {
         o = searchBlocking;
    } else if (prop == "hit.fields") {
         o = hitFields;
    } else if (prop == "hit.fields.extended") {
         o = hitFieldsExtended;
    } else if (prop == "hit.snippet.length") {
         o = hitSnippetLength;
    } else if (prop == "sort.primary") {
         o = sortPrimary;
    } else if (prop == "sort.secondary") {
         o = sortSecondary;
    } else if (prop == "sort.order") {
         o = sortAscending ?"ascending" :"descending";
    } else if (prop == "vendor.id") {
         o = "Strigi";
    } else if (prop == "vendor.version") {
         o = "0.5.1";
    } else if (prop == "vendor.display") {
         o = "Strigi Desktop Search";
    } else if (prop == "vendor.xesam") {
         o = 0;
    } else if (prop == "vendor.fieldnames") {
         o = vector<string>();
    } else if (prop == "vendor.extensions") {
         o = vector<string>();
    }
    return o;
}
XesamSession::~XesamSession() {
    for (std::list<const XesamSearch*>::const_iterator i = searches.begin();
            i != searches.end(); ++i) {
        xesam.removeSearch((*i)->name);
        delete *i;
    }
}
const std::string&
XesamSession::newSearch(const std::string& query_xml) {
    string name("XXXXXX");
    mkstemp((char*)name.c_str());
    XesamSearch* search = new XesamSearch(*this, name, query_xml);
    searches.push_back(search);
    xesam.addSearch(name, search);
    return search->name;
}
void
XesamSession::closeSearch(const XesamSearch* search) {
    xesam.removeSearch(search->name);
    searches.remove(search);
    delete search;
} 

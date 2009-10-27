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
#include "xesamclass.h"
#include <sstream>
#include <stdexcept>
using namespace std;
using namespace Strigi;

class XesamSession::Private : public XesamClass {
public:
    std::list<XesamSearch> searches;
    XesamLiveSearch& xesam;
    bool searchLive;
    bool searchBlocking;
    std::vector<std::string> hitFields;
    std::vector<std::string> hitFieldsExtended;
    uint32_t hitSnippetLength;
    std::string sortPrimary;
    std::string sortSecondary;
    bool sortAscending;
    bool hasCreatedSearches;

    Private(XesamLiveSearch& x);
    ~Private();
    Strigi::Variant setProperty(const std::string& prop,
        const Strigi::Variant& v);
    Strigi::Variant getProperty(const std::string& prop);
};
XesamSession::Private::Private(XesamLiveSearch& x) :xesam(x),
        searchLive(false),
        searchBlocking(true),
        hitSnippetLength(200),
        sortPrimary("xesam:score"),
        sortAscending(false),
        hasCreatedSearches(false) {
    hitFields.push_back("nie:url");
}
XesamSession::Private::~Private() {
    for (std::list<XesamSearch>::const_iterator i = searches.begin();
            i != searches.end(); ++i) {
        xesam.removeSearch(i->name());
    }
}
XesamSession::XesamSession(XesamLiveSearch& x) :p(new Private(x)) {
}
XesamSession::XesamSession(const XesamSession& xs) :p(xs.p) {
    p->ref();
}
XesamSession::~XesamSession() {
    p->unref();
}
void
XesamSession::operator=(const XesamSession& xs) {
    p->unref();
    p = xs.p;
    p->ref();
}
Variant
XesamSession::setProperty(const std::string& prop, const Variant& v) {
    return p->setProperty(prop, v);
}
Variant
XesamSession::Private::setProperty(const std::string& prop, const Variant& v) {
    if (hasCreatedSearches) {
        throw runtime_error("No properties can be set after a search has been "
            "created.");
    }
    Variant o;
    if (prop == "search.live") {
         if (v.type() == Variant::b_val) {
             o = false;
         } else {
             throw runtime_error("Value should be boolean.");
         }
    } else if (prop == "hit.fields") {
         if (v.type() == Variant::as_val) {
             o = hitFields = v.as();
         } else {
             throw runtime_error("Value should be a string array.");
         }
    } else if (prop == "hit.fields.extended") {
         if (v.type() == Variant::as_val) {
             o = hitFieldsExtended = v.as();
         } else {
             throw runtime_error("Value should be a string array.");
         }
    } else if (prop == "hit.snippet.length") {
         if (v.type() == Variant::i_val) {
             o = hitSnippetLength = max(v.i(), 0);
         } else {
             throw runtime_error("Value should be an integer.");
         }
    } else if (prop == "sort.primary") {
         if (v.type() == Variant::s_val) {
             o = sortPrimary = v.s();
         } else {
             throw runtime_error("Value should be a string.");
         }
    } else if (prop == "sort.secondary") {
         if (v.type() == Variant::s_val) {
             o = sortSecondary = v.s();
         } else {
             throw runtime_error("Value should be a string.");
         }
    } else if (prop == "sort.order") {
         if (v.type() == Variant::s_val) {
             sortAscending = v.s() == "ascending";
             o = sortAscending ?"ascending" :"descending";
         } else {
             throw runtime_error("Value should be a string.");
         }
    } else if (prop == "vendor.id") {
         o = getProperty(prop);
    } else {
         throw runtime_error(prop + " is an unknown property.");
    }
    return o;
}
Variant
XesamSession::getProperty(const std::string& prop) {
    return p->getProperty(prop);
}
Variant
XesamSession::Private::getProperty(const std::string& prop) {
    Variant o;
    if (prop == "search.live") {
         o = searchLive;
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
         o = (uint32_t)0;
    } else if (prop == "vendor.display") {
         o = "Strigi Desktop Search";
    } else if (prop == "vendor.xesam") {
         o = (uint32_t)90;
    } else if (prop == "vendor.ontology.fields") {
         o = vector<string>();
    } else if (prop == "vendor.ontology.contents") {
         o = vector<string>();
    } else if (prop == "vendor.ontology.sources") {
         o = vector<string>();
    } else if (prop == "vendor.extensions") {
         o = vector<string>();
    } else if (prop == "vendor.ontologies") {
         o = vector<vector<string> >();
    } else if (prop == "vendor.maxhits") {
         o = (uint32_t)100;
    }
    return o;
}
std::string
XesamSession::newSearch(const std::string& query_xml) {
    p->hasCreatedSearches = true;

    ostringstream str;
    str << "strigisearch" << random();
    string name(str.str());
    XesamSearch search(*this, name, query_xml);
    if (!search.isValid()) {
        throw runtime_error("Xesam query is invalid.");
    }
    p->searches.push_back(search);
    p->xesam.addSearch(name, search);
    return search.name();
}
void
XesamSession::closeSearch(const XesamSearch& search) {
    p->searches.remove(search);
    p->xesam.removeSearch(search.name());
}
XesamLiveSearch&
XesamSession::liveSearch() const {
    return p->xesam;
}
vector<string>&
XesamSession::hitFields() const {
    return p->hitFields;
}

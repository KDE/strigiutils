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

#include "xesamlivesearch.h"
#include <iostream>
using namespace std;

string
XesamLiveSearch::NewSession() {
    return "not implemented";
}
Variant
XesamLiveSearch::SetProperty(const std::string& session,
        const std::string& prop, const Variant& v) {
    return v;
}
void
XesamLiveSearch::CloseSession(const string& session) {
}
string
XesamLiveSearch::NewSearch(const string& session, const string& query_xml) {
    return "not implemented";
}
int32_t
XesamLiveSearch::CountHits(const string& search) {
    return -1;
}
vector<vector<Variant> >
XesamLiveSearch::GetHits(const string& search, int32_t num) {
    return vector<vector<Variant> >();
}
vector<vector<Variant> >
XesamLiveSearch::GetHitData(const string& search,
        const vector<int32_t>& hit_ids, const vector<string>& properties) {
    return vector<vector<Variant> >();
}
void
XesamLiveSearch::CloseSearch(const string& search) {
}
vector<string>
XesamLiveSearch::GetState() {
    return vector<string>();
}
void
XesamLiveSearch::HitsAdded(const std::string& search, const int32_t count) {
    for (vector<XesamLiveSearchInterface*>::const_iterator i = ifaces.begin();
            i != ifaces.end(); ++i) {
        (*i)->HitsAdded(search, count);
    }
}
void
XesamLiveSearch::HitsRemoved(const std::string& search,
        const std::vector<int32_t>& hit_ids) {
    for (vector<XesamLiveSearchInterface*>::const_iterator i = ifaces.begin();
            i != ifaces.end(); ++i) {
        (*i)->HitsRemoved(search, hit_ids);
    }
}
void
XesamLiveSearch::HitsModified(const std::string& search,
        const std::vector<int32_t>& hit_ids) {
    for (vector<XesamLiveSearchInterface*>::const_iterator i = ifaces.begin();
            i != ifaces.end(); ++i) {
        (*i)->HitsModified(search, hit_ids);
    }
}

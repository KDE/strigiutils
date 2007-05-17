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

#include "xesamlivesearchinterface.h"
#include <iostream>
using namespace std;

string
XesamLiveSearchInterface::NewSession() {
    return "not implemented";
}
Variant
XesamLiveSearchInterface::SetProperty(const std::string& session,
        const std::string& prop, const Variant& v) {
    return v;
}
void
XesamLiveSearchInterface::CloseSession(const string& session) {
}
string
XesamLiveSearchInterface::NewSearch(const string& session, const string& query_xml) {
    return "not implemented";
}
int32_t
XesamLiveSearchInterface::CountHits(const string& search) {
    return -1;
}
vector<vector<Variant> >
XesamLiveSearchInterface::GetHits(const string& search, int32_t num) {
    return vector<vector<Variant> >();
}
vector<vector<Variant> >
XesamLiveSearchInterface::GetHitData(const string& search,
        const vector<int32_t>& hit_ids, const vector<string>& properties) {
    return vector<vector<Variant> >();
}
void
XesamLiveSearchInterface::CloseSearch(const string& search) {
}
vector<string>
XesamLiveSearchInterface::GetState() {
    return vector<string>();
}

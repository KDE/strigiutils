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
#include "xesamsession.h"
#include "xesamsearch.h"
#include "query.h"
#include "queue/jobqueue.h"
#include <iostream>
#include <cstdlib>
using namespace std;
using namespace Strigi;

class XesamLiveSearch::Private {
public:
    map<string, XesamSession> sessions;
    map<string, XesamSearch> searches;
    XesamLiveSearch& xesam;
    JobQueue& queue;
    Strigi::IndexManager* const indexManager;

    Private(XesamLiveSearch& x, Strigi::IndexManager* i, JobQueue& q)
        :xesam(x), queue(q), indexManager(i) {}
    ~Private();
};
XesamLiveSearch::Private::~Private() {
}
XesamLiveSearch::XesamLiveSearch(Strigi::IndexManager* i, JobQueue& q)
        :XesamLiveSearchInterface(this), p(new Private(*this, i, q)) {}
XesamLiveSearch::~XesamLiveSearch() {
    delete p;
}
string
XesamLiveSearch::NewSession() {
    ostringstream str;
    str << "strigisession" << random();
    string name(str.str());
    mkstemp((char*)name.c_str());
    XesamSession session(*this);
    p->sessions.insert(make_pair(name, session));
    return name;
}
Strigi::Variant
XesamLiveSearch::SetProperty(const std::string& session,
        const std::string& prop, const Strigi::Variant& v) {
    Strigi::Variant out;
    map<string, XesamSession>::iterator i = p->sessions.find(session);
    if (i != p->sessions.end()) {
        out = i->second.setProperty(prop, v);
    }
    return out;
}
Strigi::Variant
XesamLiveSearch::GetProperty(const std::string& session,
        const std::string& prop) {
    Strigi::Variant out;
    map<string, XesamSession>::iterator i = p->sessions.find(session);
    if (i != p->sessions.end()) {
        out = i->second.getProperty(prop);
    }
    return out;
}
void
XesamLiveSearch::CloseSession(const string& session) {
    map<string, XesamSession>::const_iterator i = p->sessions.find(session);
    if (i != p->sessions.end()) {
        p->sessions.erase(i->first);
    }
}
string
XesamLiveSearch::NewSearch(const string& session, const string& query_xml) {
    map<string, XesamSession>::iterator i = p->sessions.find(session);
    string name;
    if (i != p->sessions.end()) {
        name = i->second.newSearch(query_xml);
    }
    return name;
}
void
XesamLiveSearch::StartSearch(const string& search) {
}
void
XesamLiveSearch::CountHits(void* msg, const string& search) {
    map<string, XesamSearch>::iterator i = p->searches.find(search);
    if (i != p->searches.end()) {
        i->second.countHits(msg);
    } else {
        CountHitsResponse(msg, -1);
    }
    //HitsAdded(search, 10);
}
void
XesamLiveSearch::CountHitsResponse(void* msg, int32_t count) {
    for (vector<XesamLiveSearchInterface*>::const_iterator i = ifaces.begin();
            i != ifaces.end(); ++i) {
        (*i)->CountHitsResponse(msg, count);
    }
}
void
XesamLiveSearch::GetHits(void*msg, const string& search, int32_t num) {
    map<string, XesamSearch>::iterator i = p->searches.find(search);
    if (i != p->searches.end()) {
        i->second.getHits(msg, num);
    } else {
        vector<vector<Strigi::Variant> > v;
        GetHitsResponse(msg, v);
    }
}
void
XesamLiveSearch::GetHitsResponse(void* msg, const vector<vector<Strigi::Variant> >& h) {
    for (vector<XesamLiveSearchInterface*>::const_iterator i = ifaces.begin();
            i != ifaces.end(); ++i) {
        (*i)->GetHitsResponse(msg, h);
    }
}
void
XesamLiveSearch::GetHitData(void* msg, const string& search,
        const vector<int32_t>& hit_ids, const vector<string>& properties) {
    map<string, XesamSearch>::iterator i = p->searches.find(search);
    if (i != p->searches.end()) {
        i->second.getHitData(hit_ids, properties);
    } else {
        vector<vector<Strigi::Variant> > v;
        GetHitDataResponse(msg, v);
    }
}
void
XesamLiveSearch::GetHitDataResponse(void* msg,
        const vector<vector<Strigi::Variant> >& v) {
    for (vector<XesamLiveSearchInterface*>::const_iterator i = ifaces.begin();
            i != ifaces.end(); ++i) {
        (*i)->GetHitDataResponse(msg, v);
    }
}
void
XesamLiveSearch::CloseSearch(const string& search) {
    map<string, XesamSearch>::const_iterator i = p->searches.find(search);
    if (i != p->searches.end()) {
        i->second.session().closeSearch(i->second); 
    }
}
vector<string>
XesamLiveSearch::GetState() {
    cerr << "XesamLiveSearch::GetState()" << endl;
    vector<string> state;
    state.push_back("IDLE");
    return state;
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
void
XesamLiveSearch::addSearch(const std::string& name, XesamSearch& search) {
    p->searches.insert(make_pair(name, search));
}
void
XesamLiveSearch::removeSearch(const std::string& name) {
    p->searches.erase(name);
}
Strigi::IndexManager*
XesamLiveSearch::indexManager() const {
    return p->indexManager;
}
JobQueue&
XesamLiveSearch::queue() const {
    return p->queue;
}

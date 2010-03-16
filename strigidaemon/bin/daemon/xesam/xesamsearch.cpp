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
#include "xesamsearch.h"
#include "xesamsession.h"
#include "xesamlivesearch.h"
#include "xesamclass.h"
#include <strigi/indexmanager.h>
#include <strigi/indexreader.h>
#include <strigi/queryparser.h>
#include <strigi/variant.h>
#include "queue/jobqueue.h"
#include "queue/job.h"
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace Strigi;

class XesamSearch::Private : public XesamClass {
public:
    const std::string name;
    std::string queryString;
    Strigi::Query query;
    XesamSession session;
    // list of requests to count to which we should reply
    std::list<void *> countMessages;
    int hitcount;
    int hitsgotten;
    bool valid;
    bool started;
    STRIGI_MUTEX_DEFINE(mutex);

    Private(XesamSession& s, const std::string& n, const std::string& q);
    ~Private();
    void getHitCount(void*);
    void setCount(int c);
    void getHits(void* msg, uint32_t num);
    void getHitData(void* msg, const std::vector<uint32_t>& hit_ids,
        const std::vector<std::string>& properties);
    void startSearch();
};

class CountJob : public Job {
public:
    XesamSearch search;

    CountJob(XesamSearch s) :search(s) {}
    void run() {
        int count = 0;
        try {
            count = search.session().liveSearch().indexManager()->indexReader()
                ->countHits(search.query());
        } catch (...) {
            cerr << "exception!" << endl;
        }
        search.setCount(count);
    }
};

class GetHitsJob : public Job {
public:
    XesamSearch search;
    void* const msg;
    const int offset;
    const int num;

    GetHitsJob(XesamSearch s, void* m, int off, int n) :search(s), msg(m),
        offset(off), num(n) {}
    void run() {
        IndexReader* reader
            = search.session().liveSearch().indexManager()->indexReader();
        const vector<Variant::Type> types(search.session().hitFields().size(),
            Variant::s_val);
        vector<vector<Variant> > v;
        reader->getHits(search.query(), search.session().hitFields(), types,
            v, offset, num);
        search.session().liveSearch().GetHitsResponse(msg, 0, v);
    }
};

class GetHitDataJob : public Job {
public:
    XesamSearch search;
    void* const msg;
    const std::vector<uint32_t> hit_ids;
    const std::vector<std::string> properties;

    GetHitDataJob(XesamSearch s, void* m, const std::vector<uint32_t>& h,
        const std::vector<std::string>& p) :search(s), msg(m),
        hit_ids(h), properties(p) {}
    void run() {
        IndexReader* reader
            = search.session().liveSearch().indexManager()->indexReader();
        const vector<Variant::Type> types(properties.size(), Variant::s_val);
        // we loop over all consecutive stretches
        vector<vector<Variant> > v;
        std::vector<uint32_t>::const_iterator i(hit_ids.begin());
        uint32_t first = hit_ids[0];
        uint32_t last = first;
        for (++i; i != hit_ids.end(); ++i) {
            if (*i != last + 1) {
                vector<vector<Variant> > sv;
                reader->getHits(search.query(), properties, types,
                    sv, first, last-first+1);
                v.insert(v.end(), sv.begin(), sv.end());
                first = last = *i;
            } else {
                last = *i;
            }
        }
        vector<vector<Variant> > sv;
        reader->getHits(search.query(), properties, types,
            sv, first, last-first+1);
        if (v.size() > 0) {
            v.insert(v.end(), sv.begin(), sv.end());
            search.session().liveSearch().GetHitDataResponse(msg, 0, v);
        } else {
            search.session().liveSearch().GetHitDataResponse(msg, 0, sv);
        }
    }
};

XesamSearch::XesamSearch(XesamSession& s, const std::string& n,
        const std::string& q) :p(new Private(s, n, q)) {
}
XesamSearch::XesamSearch(const XesamSearch& xs) :p(xs.p) {
    p->ref();
}
XesamSearch::XesamSearch(Private* xsp) :p(xsp) {
    p->ref();
}
XesamSearch::~XesamSearch() {
    p->unref();
}
void
XesamSearch::operator=(const XesamSearch& xs) {
    p->unref();
    p = xs.p;
    p->ref();
}
XesamSearch::Private::Private(XesamSession& s, const std::string& n,
        const std::string& q) :name(n), session(s), hitcount(-1), hitsgotten(0),
            started(false) {
    STRIGI_MUTEX_INIT(&mutex);

    // simple check to make sure this is a xesam query
    valid = q.find("<request") != string::npos;
    if (!valid) {
        throw runtime_error("Error parsing query: not a valid XML Xesam query");
    }

    query = Strigi::QueryParser::buildQuery(q);
    valid = query.valid();
    if (!valid) {
        throw runtime_error("Error parsing query.");
    }
}
XesamSearch::Private::~Private() {
    STRIGI_MUTEX_DESTROY(&mutex);
}
void
XesamSearch::getHitCount(void* msg) {
    p->getHitCount(msg);
}
void
XesamSearch::Private::getHitCount(void* msg) {
    if (!started) {
        throw runtime_error("Search has not been started.");
    } else if (!valid) {
        // send an error message
        throw runtime_error("Search is not valid.");
    }
    STRIGI_MUTEX_LOCK(&mutex);
    if (hitcount == -1) {
        // count is not yet know, put msg in list of msgs to be sent when
        // we know the count
        countMessages.push_back(msg);
    } else {
        // we know the count and can send it
        session.liveSearch().GetHitCountResponse(msg, 0, hitcount);
    }
    STRIGI_MUTEX_UNLOCK(&mutex);
}
void
XesamSearch::getHits(void* msg, uint32_t num) {
    p->getHits(msg, num);
}
void
XesamSearch::Private::getHits(void* msg, uint32_t num) {
    if (!started) {
        throw runtime_error("Search has not been started.");
    } else if (!valid) {
        // send an error message
        throw runtime_error("Search is not valid.");
    }
    uint32_t sofar = hitsgotten;
    hitsgotten += num;
    GetHitsJob* job = new GetHitsJob(XesamSearch(this), msg, sofar, num);
    bool ok = session.liveSearch().queue().addJob(job);
    if (!ok) {
        delete job;
        throw runtime_error("Error processing request.");
    }
}
void
XesamSearch::getHitData(void* msg, const std::vector<uint32_t>& hit_ids,
        const std::vector<std::string>& properties) {
    p->getHitData(msg, hit_ids, properties);
}
void
XesamSearch::Private::getHitData(void* msg,
        const std::vector<uint32_t>& hit_ids,
        const std::vector<std::string>& properties) {
    if (!started) {
        throw runtime_error("Search has not been started.");
    } else if (!valid) {
        // send an error message
        throw runtime_error("Search is not valid.");
    }
    if (hit_ids.size() == 0) {
        vector<vector<Variant> > v;
        session.liveSearch().GetHitDataResponse(msg, 0, v);
        return;
    }
    GetHitDataJob* job = new GetHitDataJob(XesamSearch(this), msg, hit_ids,
        properties);
    bool ok = session.liveSearch().queue().addJob(job);
    if (!ok) {
        delete job;
        throw runtime_error("Error processing request.");
    }
}
void
XesamSearch::setCount(int c) {
    p->setCount(c);
}
void
XesamSearch::Private::setCount(int c) {
    STRIGI_MUTEX_LOCK(&mutex);
    if (valid && hitcount == -1) {
        // reply all the outstanding requests
        hitcount = c;
        for (list<void*>::const_iterator i = countMessages.begin();
                 i != countMessages.end(); ++i) {
            session.liveSearch().GetHitCountResponse(*i, 0, hitcount);
        }
        countMessages.clear();
    }
    STRIGI_MUTEX_UNLOCK(&mutex);
    if (c > 0) {
        session.liveSearch().HitsAdded(name, c);
    }
    session.liveSearch().SearchDone(name);
}
string
XesamSearch::name() const {
    return p->name;
}
Strigi::Query
XesamSearch::query() const {
    return p->query;
}
XesamSession
XesamSearch::session() const {
    return p->session;
}
bool
XesamSearch::isValid() const {
    return p->valid;
}
void
XesamSearch::startSearch() {
    p->startSearch();
}
void
XesamSearch::Private::startSearch() {
    if (started) {
        throw runtime_error("Search was already started.");
    }
    started = true;
    // retrieve number of hits already
    CountJob* job = new CountJob(XesamSearch(this));
    bool ok = session.liveSearch().queue().addJob(job);
    if (!ok) {
        delete job;
        throw runtime_error("Error starting search.");
    }
}

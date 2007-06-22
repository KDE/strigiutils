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
#include "indexmanager.h"
#include "indexreader.h"
#include "queryparser.h"
#include "variant.h"
#include "queue/jobqueue.h"
#include "queue/job.h"
#include <iostream>
using namespace std;

class XesamSearch::Private : public XesamClass {
public:
    const std::string name;
    const std::string queryString;
    const Strigi::Query query;
    XesamSession session;
    // list of requests to count to which we should reply
    std::list<void *> countMessages;
    int hitcount;
    bool valid;
    STRIGI_MUTEX_DEFINE(mutex);

    Private(XesamSession& s, const std::string& n, const std::string& q);
    ~Private();
    void countHits(void*);
    void setCount(int c);
};

class CountJob : public Job {
public:
    XesamSearch search;

    CountJob(XesamSearch s) :search(s) {}
    void run() {
        int count = search.session().liveSearch().indexManager()->indexReader()
            ->countHits(search.query());
        search.setCount(count);
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
        const std::string& q)
        :name(n), queryString(q), query(Strigi::QueryParser::buildQuery(q)),
         session(s) {
    hitcount = -1;
    valid = true;
    STRIGI_MUTEX_INIT(&mutex);
    // retrieve number of hits already
    CountJob* job = new CountJob(XesamSearch(this));
    valid = session.liveSearch().queue().addJob(job);
    if (valid) {
        STRIGI_MUTEX_INIT(&mutex);
    } else {
        delete job;
    }
}
XesamSearch::Private::~Private() {
    STRIGI_MUTEX_DESTROY(&mutex);
}
void
XesamSearch::countHits(void* msg) {
    p->countHits(msg);
}
void
XesamSearch::Private::countHits(void* msg) {
    if (!valid) {
        session.liveSearch().CountHitsResponse(msg, -1);
    } else {
        STRIGI_MUTEX_LOCK(&mutex);
        if (hitcount == -1) {
            // count is not yet know, put msg in list of msgs to be sent when
            // we know the count
            countMessages.push_back(msg);
        } else {
            // we know the count and can send it
            session.liveSearch().CountHitsResponse(msg, hitcount);
        }
        STRIGI_MUTEX_UNLOCK(&mutex);
    }
}
vector<vector<Variant> >
XesamSearch::getHits(int32_t num) {
    vector<vector<Variant> > v;
    return v;
}
std::vector<std::vector<Variant> >
XesamSearch::getHitData(const std::vector<int32_t>& hit_ids,
        const std::vector<std::string>& properties) {
    vector<vector<Variant> > v;
    return v;
}
void
XesamSearch::setCount(int c) {
    p->setCount(c);
}
void
XesamSearch::Private::setCount(int c) {
    STRIGI_MUTEX_LOCK(&mutex);
    if (valid && hitcount == -1) {
        hitcount = c;
        for (list<void*>::const_iterator i = countMessages.begin();
                 i != countMessages.end(); ++i) {
            session.liveSearch().CountHitsResponse(*i, hitcount);
        }
        countMessages.clear();
    }
    STRIGI_MUTEX_UNLOCK(&mutex);
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

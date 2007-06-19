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
#include "indexmanager.h"
#include "indexreader.h"
#include "queryparser.h"
#include "variant.h"
#include "queryparser.h"
#include "queue/jobqueue.h"
#include "queue/job.h"
#include <iostream>
using namespace std;

class CountJob : public Job {
public:
    XesamSearch& search;
    void* msg;
    CountJob(XesamSearch& s, void* m) :search(s), msg(m) {}
    ~CountJob() {
    }
    void run() {
        //int count = search.session.liveSearch().indexManager()->indexReader()
        //    ->countHits(search.query);
        sleep(5);
        cerr << "going to reply" << endl;
        search.session.liveSearch().CountHitsResponse(msg, 10);
    }
};

XesamSearch::XesamSearch(XesamSession& s, const std::string& n,
        const std::string& q)
        :name(n), queryString(q), query(Strigi::QueryParser::buildQuery(q)),
         session(s) {
}
void
XesamSearch::countHits(void* msg) {
    CountJob* job = new CountJob(*this, msg);
    if (!session.liveSearch().queue().addJob(job)) {
        delete job;
    }
    //session.liveSearch().CountHitsResponse(msg, 10);
//    return session.liveSearch().indexManager()->indexReader()->countHits(query);
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

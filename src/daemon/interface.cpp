/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#include "interface.h"
#include "indexreader.h"
#include "indexmanager.h"
#include "indexscheduler.h"
#include "query.h"
#include <sstream>
using namespace std;
using namespace jstreams;

int
Interface::countHits(const string& query) {
    Query q(query, -1, 0);
    int count = manager.getIndexReader()->countHits(q);
    return count;
}
ClientInterface::Hits
Interface::getHits(const string& query, int max, int off) {
    Query q(query, max, off);
    Hits hits;
    hits.hits = manager.getIndexReader()->query(q);
    // highlight the hits in the results
    vector<IndexedDocument>::iterator i;
    for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
        i->fragment = q.highlight(i->fragment);
    }
    return hits;
}
map<string, string>
Interface::getStatus() {
    map<string,string> status;
    status["Status"]=scheduler.getState();
    ostringstream out;
    out << scheduler.getQueueSize();
    status["Documents in queue"]= out.str();
    out.str("");
    IndexReader* reader = manager.getIndexReader();
    out << reader->countDocuments();
    status["Documents indexed"]= out.str();
    out.str("");
    out << reader->countWords();
    status["Unique words indexed"] = out.str();
    out.str("");
    out << reader->getIndexSize()/1024/1024;
    status["Index size"] = out.str()+" MB";
    return status;
}
string
Interface::stopDaemon() {
    active = false;
    scheduler.stop();
    return "";
}
string
Interface::startIndexing() {
    scheduler.startIndexing();
    return "";
}
string
Interface::stopIndexing() {
    scheduler.stopIndexing();
    return "";
}
set<string>
Interface::getIndexedDirectories() {
    return scheduler.getIndexedDirectories();
}
string
Interface::setIndexedDirectories(set<string> dirs) {
    scheduler.setIndexedDirectories(dirs);
    return "";
}

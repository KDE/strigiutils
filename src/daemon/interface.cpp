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
#include "strigiconfig.h"
#include "interface.h"
#include "indexreader.h"
#include "combinedindexmanager.h"
#include "indexwriter.h"
#include "indexscheduler.h"
#include "eventlistener.h"
#include "streamanalyzer.h"
#include "analysisresult.h"
#include "analyzerconfiguration.h"
#include "stringstream.h"
#include "query.h"
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
using namespace std;
using namespace Strigi;

Interface::Interface(CombinedIndexManager& m, IndexScheduler& s)
        :manager(m), scheduler(s) {
    eventListener = NULL;
}
void
Interface::setEventListener (EventListener* eListener) {
    eventListener = eListener;
}
int
Interface::countHits(const string& query) {
    QueryParser parser;
    Query q = parser.buildQuery(query, -1, 0);
    int count = manager.indexReader()->countHits(q);
    return count;
}
ClientInterface::Hits
Interface::getHits(const string& query, uint32_t max, uint32_t off) {
    QueryParser parser;
    Query q = parser.buildQuery(query, max, off);
    Hits hits;
    hits.hits = manager.indexReader()->query(q);
    // highlight the hits in the results
    vector<IndexedDocument>::iterator i;
    for (i = hits.hits.begin(); i != hits.hits.end(); ++i) {
        i->fragment = q.highlight(i->fragment);
    }
    return hits;
}
vector<string>
Interface::getBackEnds() {
    return manager.backEnds();
}
map<string, string>
Interface::getStatus() {
    map<string,string> status;
    status["Status"]=scheduler.getStateString();
    ostringstream out;
    out << scheduler.getQueueSize();
    status["Documents in queue"]= out.str();
    out.str("");
    IndexReader* reader = manager.indexReader();
    out << reader->countDocuments();
    status["Documents indexed"]= out.str();
    out.str("");
    out << reader->countWords();
    status["Unique words indexed"] = out.str();
    out.str("");
    out << reader->indexSize()/1024/1024;
    status["Index size"] = out.str()+" MB";
    return status;
}
string
Interface::stopDaemon() {
    active = false;
    // signal to all threads to quit. Do not use raise() here, because it will
    // cause a hang
    kill(getpid(), SIGINT);
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
    if (eventListener != NULL)
        eventListener->setIndexedDirectories( dirs);

    scheduler.setIndexedDirectories(dirs);
    return "";
}
void
Interface::setFilters(const vector<pair<bool,string> >& rules) {
    scheduler.getIndexerConfiguration().setFilters(rules);
}
vector<pair<bool,string> >
Interface::getFilters() {
    return scheduler.getIndexerConfiguration().filters();
}
set<string>
Interface::getIndexedFiles() {
    map<string, time_t> indexedfiles = manager.indexReader()->files(0);
    set<string> r;

    for (map<string, time_t>::iterator iter = indexedfiles.begin(); iter != indexedfiles.end(); iter++)
        r.insert (iter->first);

    return r;
}
void
Interface::indexFile(const string &path, uint64_t mtime,
        const vector<char>& content) {
    // TODO if the file is already there, remove it first
    IndexWriter* writer = manager.indexWriter();
    vector<string> paths;
    paths.push_back(path);
    writer->deleteEntries(paths);
    AnalyzerConfiguration ic;
    StreamAnalyzer streamindexer(ic);
    StringInputStream sr(&content[0], content.size(), false);
    AnalysisResult idx(path, mtime, *writer, streamindexer);
    idx.index(&sr);
}
vector<string>
Interface::getFieldNames() {
    return manager.indexReader()->fieldNames();
}
vector<pair<string, uint32_t> >
Interface::getHistogram(const string& query, const string& field,
        const string& labeltype) {
    return manager.indexReader()->histogram(query, field, labeltype);
}
int32_t
Interface::countKeywords(const string& keywordmatch,
        const vector<string>& fieldnames) {
    return 0;
}
vector<string>
Interface::getKeywords(const string& keywordmatch,
        const vector<string>& fieldnames,
        uint32_t max, uint32_t offset) {
    return vector<string>();
}

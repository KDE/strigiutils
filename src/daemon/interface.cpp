#include "interface.h"
#include "indexreader.h"
#include "indexmanager.h"
#include "indexscheduler.h"
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

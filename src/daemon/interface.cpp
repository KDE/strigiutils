#include "interface.h"
#include "indexreader.h"
#include "indexmanager.h"
#include "indexscheduler.h"
#include <sstream>
using namespace std;
using namespace jstreams;

vector<string>
Interface::query(const string& query) {
    vector<IndexedDocument> docs = manager.getIndexReader()->query(query);
    vector<string> r;
    for (uint i=0; i < docs.size(); ++i) {
        r.push_back(docs[i].filepath);
    }
    return r;
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
    status["Index size"] = out.str()+" Mb";
    return status;
}
std::string
Interface::stopDaemon() {
    active = false;
    scheduler.stop();
    return "";
}
std::string
Interface::startIndexing() {
    scheduler.startIndexing();
    return "";
}
std::string
Interface::stopIndexing() {
    scheduler.stopIndexing();
    return "";
}
std::vector<std::string>
Interface::getIndexedDirectories() {
    vector<string> dirs;
    const set<string>& d = scheduler.getIndexedDirectories();
    set<string>::const_iterator i;
    for (i=d.begin(); i!=d.end(); ++i) {
        dirs.push_back(*i);
    }
    return dirs;
}
std::string
Interface::setIndexedDirectories(std::vector<std::string> d) {
    set<string> dirs;
    for (uint i=0; i<d.size(); ++i) {
        dirs.insert(d[i]);
    }
    scheduler.setIndexedDirectories(dirs);
    return "";
}

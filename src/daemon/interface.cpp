#include "interface.h"
#include "indexreader.h"
#include "indexmanager.h"
#include "indexscheduler.h"
#include <sstream>
using namespace std;
using namespace jstreams;

vector<string>
Interface::query(const string& query) {
    vector<string> x;
    x.push_back("impl");
    return manager.getIndexReader()->query(query);
}
map<string, string>
Interface::getStatus() {
    map<string,string> status;
    status["Status"]="running";
    ostringstream out;
    out << scheduler.getQueueSize();
    status["Documents in queue"]= out.str();
    out.str("");
    IndexReader* reader = manager.getIndexReader();
    out << reader->countDocuments();
    status["Documents indexed"]= out.str();
    return status;
}

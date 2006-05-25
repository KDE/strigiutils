#include "interface.h"
#include "indexreader.h"
#include "indexmanager.h"
#include <sstream>
using namespace std;
using namespace jstreams;

vector<string>
Interface::query(const string& query) {
    vector<string> x;
    x.push_back("impl");
    return manager->getIndexReader()->query(query);
}
map<string, string>
Interface::getStatus() {
    static int calls = 0;
    map<string,string> status;
    status["status"]="running";
    ostringstream out;
    out << ++calls;
    status["status calls"]= out.str();
    return status;
}

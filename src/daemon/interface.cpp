#include "interface.h"
#include "indexreader.h"
#include "indexmanager.h"
using namespace std;
using namespace jstreams;

vector<string>
Interface::query(const string& query) {
    vector<string> x;
    x.push_back("impl");
    return manager->getIndexReader()->query(query);
}

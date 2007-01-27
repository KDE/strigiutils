#include "tagmapping.h"
#include <iostream>
#include <fstream>
using namespace std;

TagMapping::TagMapping(const char* path) {
    if (path == 0) return;
    ifstream file(path);
    string line;
    for (;;) {
        getline(file, line);
        if (!file.good()) {
            break;
        }
        string::size_type p = line.find('\t');
        if (p != string::npos) {
            mapping[line.substr(0, p)] = line.substr(p+1);
        } else {
            p = line.find(':');
            if (p != string::npos) {
                namespaces[line.substr(0, p)] = line.substr(p+1);
            }
        }
    }
}


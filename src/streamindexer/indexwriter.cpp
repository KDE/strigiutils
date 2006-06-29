#include "indexwriter.h"
using namespace jstreams;
using namespace std;
string
Indexable::getExtension() const {
    int p1 = name.rfind('.');
    int p2 = name.rfind('/');
    if (p1 != string::npos && (p2 == string::npos || p1 > p2)) {
        return name.substr(p1+1);
    }
    return "";
}
string
Indexable::getFileName() const {
    int p = name.rfind('/');
    if (p != string::npos) {
        return name.substr(p+1);
    }
    return "";
}

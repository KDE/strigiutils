#include "query.h"
#include "queryparser.h"
#include <iostream>
#include <cassert>
using namespace Strigi;
using namespace std;

int
querytests(int argc, char* argv[]) {
    QueryParser p;

    Query q = p.buildQuery("hi");
    assert(q.fields().size() == 0);
    assert(q.subQueries().size() == 0);
    assert(q.term().string() == "hi");

    q = p.buildQuery("hey ho");
    cerr << q.subQueries().size() << endl;
    assert(q.subQueries().size() == 2);

    q = p.buildQuery("he* content.mime_type:image*");
    assert(q.subQueries().size() == 2);
    return 0;
}

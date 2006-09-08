#include "processinputstream.h"
#include "stringreader.h"
using namespace jstreams;

int
main(int argc, char** argv) {
    std::vector<std::string> a;
    for (int i=1; i<argc; ++i) {
        a.push_back(argv[i]);
    }

    ProcessInputStream x(a);
    const char* d;
    int32_t n = x.read(d, 1, 1);
    while (n > 0) {
        printf("%.*s", n, d);
        n = x.read(d, 1, 1);
    }

    std::vector<std::string> b;
    b.push_back("/bin/cat");
    StringInputStream s("hallo\n");
    ProcessInputStream y(b, &s);
    n = y.read(d, 1, 1);
    while (n > 0) {
        printf("%.*s", n, d);
        n = y.read(d, 1, 1);
    }


    return 0;
}

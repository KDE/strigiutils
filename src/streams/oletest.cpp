#include "fileinputstream.h"
#include "oleinputstream.h"
using namespace Strigi;

int
main(int argc, char** argv) {
    for (int i=1; i<argc; ++i) {
        fprintf(stderr, "%s\n", argv[i]);
        FileInputStream f(argv[i]);
        OleInputStream o(&f);
    }
    return 0;
}

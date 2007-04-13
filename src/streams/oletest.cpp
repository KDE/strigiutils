#include "fileinputstream.h"
#include "oleinputstream.h"
using namespace Strigi;

int
main(int argc, char** argv) {
    for (int i=1; i<argc; ++i) {
        fprintf(stderr, "%s\n", argv[i]);
        FileInputStream f(argv[i]);
        OleInputStream o(&f);
        InputStream* j = o.nextEntry();
        while (j) {
            const char* data;
            int64_t s = 0;
            int32_t n = j->read(data, 1, 1);
            while (n > 0) {
                s += n;
                n = j->read(data, 11111111, 11111);
            }
            fprintf(stderr, "out %lli %lli %i\n", s, j->size(), n);
            j = o.nextEntry();
        }
    }
    return 0;
}

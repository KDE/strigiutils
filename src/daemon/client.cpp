/* a client in the unix domain */
#include "socketclient.h"

int
main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage %s query\n", argv[0]);
        return -1;
    }
    SocketClient c;
    c.setSocketName("/tmp/katsocket");
    std::vector<std::string> files = c.query(argv[1]);
    if (files.size() == 0) {
        printf("Error: %s\n", c.getError().c_str());
    } else {
        for (uint i=0; i<files.size(); ++i) {
            printf("%i\t%s\n", i, files[i].c_str());
        }
    }
}


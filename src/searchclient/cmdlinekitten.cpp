#include "asyncsocketclient.h"
using namespace std;

int
main(int argc, char** argv) {
    if (argc < 2) return -1;
    string query;
    for (int i=1; i<argc; ++i) {
        query += argv[i];
        query += " ";
    }
    string homedir = getenv("HOME");
    AsyncSocketClient client;
    client.setSocketPath(homedir+"/.kitten/socket");
    bool ok = client.countHits(query.c_str());
    if (ok) {
        while (!client.statusChanged()) {
            printf(".");
            fflush(stdout);
            struct timespec sleeptime;
            sleeptime.tv_sec = 0;
            sleeptime.tv_nsec = 1;
            nanosleep(&sleeptime, 0);
        }
        printf("hits: %i\n", client.getHitCount());
    }
            
    return 0;
}

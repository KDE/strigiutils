#include "asyncsocketclient.h"
#include <signal.h>
using namespace std;

void
quit_daemon(int i) {
    printf("signal %i\n", i);
}

struct sigaction quitaction;

void
set_quit_on_signal(int signum) {
    quitaction.sa_handler = quit_daemon;
    sigaction(signum, &quitaction, 0);
}

int
main(int argc, char** argv) {
    if (argc < 2) return -1;
    set_quit_on_signal(SIGABRT);
    string query;
    for (int i=1; i<argc; ++i) {
        query += argv[i];
        query += " ";
    }
    string homedir = getenv("HOME");
    AsyncSocketClient client;
    client.setSocketPath(homedir+"/.strigi/socket");
    bool ok = client.query(query.c_str());
    if (ok) {
        while (!client.statusChanged()) {
            printf(".");
            fflush(stdout);
            struct timespec sleeptime;
            sleeptime.tv_sec = 0;
            sleeptime.tv_nsec = 1;
            nanosleep(&sleeptime, 0);
        }
        printf("hits: %i\n", client.getHits().hits.size());
    }
            
    return 0;
}

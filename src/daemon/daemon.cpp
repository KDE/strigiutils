#include "socketserver.h"
#include "interface.h"
#ifdef HAVE_CLUCENELIB
#include "cluceneindexmanager.h"
#else
#include "sqliteindexmanager.h"
#endif
#include "indexscheduler.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <cstdio>
#include <cerrno>

using namespace jstreams;
using namespace std;

IndexScheduler scheduler;

void
quit_daemon(int) {
    static int interruptcount = 0;
    switch (++interruptcount) {
    case 1:
        scheduler.stop();
        break;
    case 2:
        scheduler.terminate();
        break;
    case 3:
    default:
        exit(1);
    }
}

struct sigaction quitaction;

void
set_quit_on_signal(int signum) {
    quitaction.sa_handler = quit_daemon;
    sigaction(signum, &quitaction, 0);
}

/**
 * Initialize a directory for storing the index data and the socket.
 * Make sure it is well protected from peeping eyes.
 **/
bool
initializeDir(const string& dir) {
    struct stat s;
    // check that the directory exists
    int r = stat(dir.c_str(), &s);
    if (r == -1) {
        if (errno == ENOENT) {
            // the directory does not exist
            r = mkdir(dir.c_str(), 0700);
            if (r == -1) {
                perror(dir.c_str());
                return false;
            }
        } else {
            perror(dir.c_str());
            return false;
        }
    }
    return true;
}
int
main(int argc, char** argv) {
    set_quit_on_signal(SIGINT);
    set_quit_on_signal(SIGQUIT);
    set_quit_on_signal(SIGTERM);

    string homedir = getenv("HOME");
    string daemondir = homedir+"/.kitten";
    string lucenedir = daemondir+"/lucene";
    string dbfile = daemondir+"/sqlite.db";
    string socketpath = daemondir+"/socket";
    string dirtoindex = homedir;

    // initialize the directory for the daemon data
    if (!initializeDir(daemondir)) {
        exit(1);
    }
    if (!initializeDir(lucenedir)) {
        exit(1);
    }

    IndexManager* index;
#ifdef HAVE_CLUCENELIB
    index = new CLuceneIndexManager(lucenedir);
#else
    // initialize the storage manager, for now we only use sqlite
    index = new SqliteIndexManager(dbfile.c_str());
#endif
    scheduler.setDirToIndex(dirtoindex);
    scheduler.setIndexManager(index);


    // start the indexer thread
    scheduler.start();

    // listen for requests
    Interface interface(*index, scheduler);
    SocketServer server(&interface);
    server.setSocketName(socketpath.c_str());
    if (!server.start()) {
        scheduler.stop();
    }

    // close the indexmanager
    delete index;
}


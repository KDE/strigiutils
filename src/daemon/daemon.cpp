#include "socketserver.h"
#include "interface.h"
#ifdef HAVE_CLUCENE
#include "cluceneindexmanager.h"
#endif
#ifdef HAVE_XAPIAN
#include "xapianindexmanager.h"
#endif
#ifdef HAVE_ESTRAIER
#include "estraierindexmanager.h"
#endif
#ifdef HAVE_SQLITE
#include "sqliteindexmanager.h"
#endif
#include "indexscheduler.h"
#include <fstream>
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
set<string>
readdirstoindex(const string& file) {
    set<string> dirs;
    ifstream f(file.c_str());
    string s;
    do {
        getline(f, s);
        if (s.size()) {
            dirs.insert(s);
        }
    } while (!f.eof() && f.good());
    f.close();
    return dirs;
}
void
savedirstoindex(const string& file, const set<string> &dirs) {
    ofstream f(file.c_str());
    set<string>::const_iterator i;
    for (i=dirs.begin(); i!=dirs.end(); ++i) {
        f << *i << endl;
    }
    f.close();
}

int
main(int argc, char** argv) {
    set_quit_on_signal(SIGINT);
    set_quit_on_signal(SIGQUIT);
    set_quit_on_signal(SIGTERM);

    string homedir = getenv("HOME");
    string daemondir = homedir+"/.kitten";
    string lucenedir = daemondir+"/lucene";
    string estraierdir = daemondir+"/estraier";
    string xapiandir = daemondir+"/xapian";
    string dbfile = daemondir+"/sqlite.db";
    string dirsfile = daemondir+"/dirstoindex";
    string socketpath = daemondir+"/socket";

    // initialize the directory for the daemon data
    if (!initializeDir(daemondir)) {
        exit(1);
    }
    if (!initializeDir(lucenedir)) {
        exit(1);
    }
    if (!initializeDir(estraierdir)) {
        exit(1);
    }

    // initialize the storage manager
    IndexManager* index = 0;
#ifdef HAVE_CLUCENE
    if (index == 0) {
        index = new CLuceneIndexManager(lucenedir);
    }
#endif
#ifdef HAVE_XAPIAN
    if (index == 0) {
        index = new XapianIndexManager(xapiandir.c_str());
    }
#endif
#ifdef HAVE_ESTRAIER
    if (index == 0) {
        index = new EstraierIndexManager(estraierdir.c_str());
    }
#endif
#ifdef HAVE_SQLITE
    if (index == 0) {
        index = new SqliteIndexManager(dbfile.c_str());
    }
#endif
    set<string> dirs = readdirstoindex(dirsfile);
    scheduler.setIndexedDirectories(dirs);
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
    dirs = scheduler.getIndexedDirectories();
    savedirstoindex(dirsfile, dirs);

    // close the indexmanager
    delete index;
}


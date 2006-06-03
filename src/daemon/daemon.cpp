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
    string lucenedir = daemondir+"/clucene";
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

    // determine the right index manager
    map<string, IndexManager*(*)(const char*)> factories;
#ifdef HAVE_ESTRAIER
    factories["estraier"] = createEstraierIndexManager;
#endif
#ifdef HAVE_CLUCENE
    factories["clucene"] = createCLuceneIndexManager;
#endif
#ifdef HAVE_XAPIAN
    factories["xapian"] = createXapianIndexManager;
#endif
#ifdef HAVE_SQLITE
    factories["sqlite"] = createSqliteIndexManager;
#endif
    const char *backend = "estraier";
    if (argc > 1) backend = argv[1];
    map<string, IndexManager*(*)(const char*)>::const_iterator f
        = factories.find(backend);
    if (f == factories.end()) {
        f = factories.begin();
    }
    string indexdir = daemondir + "/" + f->first;
    IndexManager* index = f->second(indexdir.c_str());


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


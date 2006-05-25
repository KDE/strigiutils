#include "socketserver.h"
#include "interface.h"
#include "sqliteindexmanager.h"
#include "sqliteindexreader.h"
#include "filelister.h"
#include "streamindexer.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <list>
#include <cstdio>
#include <cerrno>

// linux specific includes for setting the indexer priority to low
#include <sched.h>
#include <sys/resource.h>
#ifdef HAVE_LINUXIOPRIO
#include <linux/ioprio.h>
#endif

using namespace jstreams;
using namespace std;

pthread_mutex_t stacklock = PTHREAD_MUTEX_INITIALIZER;
std::list<std::string> filestack;
bool daemon_run = true;

/* function prototypes and global variables */
void *do_chld(void *);


int interruptcount = 0;
void
quit_daemon(int) {
    if (++interruptcount >= 3) exit(1);
    daemon_run = false;
}

struct sigaction quitaction;

void
set_quit_on_signal(int signum) {
    quitaction.sa_handler = quit_daemon;
    sigaction(signum, &quitaction, 0);
}

void
shortsleep(long nanoseconds) {
    // set sleep time
    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = nanoseconds;
    nanosleep(&sleeptime, 0);
}
map<string, time_t> dbfiles;
map<string, time_t> toindex;
bool
addFileCallback(const string& path, const char *filename, time_t mtime) {
    if (!daemon_run) return false;
    // only read files that do not start with '.'
    if (*filename == '.') return true;

    std::string filepath(path+filename);

    map<string, time_t>::iterator i = dbfiles.find(filepath);
    if (i == dbfiles.end()) {
        toindex[filepath] = mtime;
    } else {
        if (i->second != mtime) {
            // signal that file must be updated
            toindex[filepath] = -1;
        }
        dbfiles.erase(i);
    }
    return true;
}
string homedir;
#include <errno.h>
void *
indexloop(void *i) {
    // give this thread job batch job priority
    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = 0;
#ifndef SCHED_BATCH
#define SCHED_BATCH 3
#endif
    int r = sched_setscheduler(0, SCHED_BATCH, &param);
    if (r != 0) {
        // fall back to renice if SCHED_BATCH is unknown
        r = setpriority(PRIO_PROCESS, 0, 20);
        if (r==-1) printf("error setting priority: %s\n", strerror(errno));
        //nice(20);
    }
#ifdef HAVE_LINUXIOPRIO
    sys_ioprio_set(IOPRIO_WHO_PROCESS, 0, IOPRIO_CLASS_IDLE);
#endif

    IndexManager* index = (IndexManager*)i;
    IndexReader* reader = index->getIndexReader();

    dbfiles = reader->getFiles(0);
    printf("%i real files in the database\n", dbfiles.size()); 
    IndexWriter* writer = index->getIndexWriter();
    StreamIndexer* streamindexer = new StreamIndexer(writer);

    // first loop through all files
    FileLister lister;
    lister.setCallbackFunction(&addFileCallback);
    printf("going to index\n");
    lister.listFiles(homedir.c_str());
    printf("%i real files left in the database\n", dbfiles.size()); 
    printf("%i files to add or update\n", toindex.size()); 

    map<string,time_t>::iterator it = toindex.begin();
    while (daemon_run && it != toindex.end()) {
        if (it->second == -1) {
        //    writer->erase(it->first);
        }
        streamindexer->indexFile(it->first);
        toindex.erase(it++);
    }

/*
    while (daemon_run) {
        shortsleep(100000000);
        pthread_mutex_lock(&stacklock);
        if (filestack.size())
            printf("doing %i file\n", filestack.size());
        while (filestack.size()) {
            std::string file = filestack.front();
            filestack.pop();
            filestack.push_front(file);
        }
        pthread_mutex_unlock(&stacklock);
    }*/
    printf("stopping indexer\n");
    return (void*)0;
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

    homedir = getenv("HOME");
    string daemondir = homedir+"/.kitten";
    string dbfile = daemondir+"/sqlite.db";
    string socketpath = daemondir+"/socket";

    // initialize the directory for the daemon data
    if (!initializeDir(daemondir)) {
        exit(1);
    }

    // initialize the storage manager, for now we only use sqlite
    SqliteIndexManager* index = new SqliteIndexManager(dbfile.c_str());

    // start the indexer thread
    pthread_t indexthread;
    int r = pthread_create(&indexthread, NULL, indexloop, index);
    if (r < 0) {
        printf("cannot create thread\n");
        return 1;
    }

    // listen for requests
    Interface interface(index);
    SocketServer server(&interface);
    server.setSocketName(socketpath.c_str());
    server.start();

    // wait for the indexer to finish
    pthread_join(indexthread, 0);

    // close the indexmanager
    delete index;
}


/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "jstreamsconfig.h"
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
#ifdef HAVE_INOTIFY
#include "inotifylistener.h"
#endif
#include "eventlistenerqueue.h"

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cstdio>
#include <cerrno>

using namespace jstreams;
using namespace std;

IndexScheduler scheduler;

#ifdef HAVE_INOTIFY
InotifyListener inotifyListener;
#endif

void
quit_daemon(int) {
    static int interruptcount = 0;
    switch (++interruptcount) {
    case 1:
#ifdef HAVE_INOTIFY
        inotifyListener.stop();
#endif
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
FILE*
aquireLock(const char* lockfile, struct flock& lock) {
    FILE* f = fopen(lockfile, "w");
    if (f == 0) {
        fprintf(stderr, strerror(errno));
        return 0;
    }
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    int r = fcntl(fileno(f), F_SETLK, &lock);
    if (r == -1) {
        fprintf(stderr, strerror(errno));
        fclose(f);
        return 0;
    }
    return f;
}
void
releaseLock(FILE* f, struct flock& lock) {
    lock.l_type = F_UNLCK;
    fcntl(fileno(f), F_SETLK, &lock);
    fclose(f);
}
int
main(int argc, char** argv) {
    // set up the directory paths
    string homedir = getenv("HOME");
    string daemondir = homedir+"/.strigi";
    string lockfilename = daemondir+"/lock";
    string lucenedir = daemondir+"/clucene";
    string estraierdir = daemondir+"/estraier";
    string xapiandir = daemondir+"/xapian";
    string dbfile = daemondir+"/sqlite.db";
    string dirsfile = daemondir+"/dirstoindex";
    string socketpath = daemondir+"/socket";

    // initialize the directory for the daemon data
    if (!initializeDir(daemondir)) {
        fprintf(stderr, "Could not initialize the daemon directory.\n");
        exit(1);
    }
    if (!initializeDir(lucenedir)) {
        fprintf(stderr, "Could not initialize the clucene directory.\n");
        exit(1);
    }
    if (!initializeDir(estraierdir)) {
        fprintf(stderr, "Could not initialize the estraier directory.\n");
        exit(1);
    }

    // check that no other daemon is running
    struct flock lock;
    FILE* lockfile = aquireLock(lockfilename.c_str(), lock);
    if (lockfile == 0) {
        printf("Daemon cannot run: the file %s is locked.\n",
            lockfilename.c_str());
        exit(1);
    } 

    // set up signal handling 
    set_quit_on_signal(SIGINT);
    set_quit_on_signal(SIGQUIT);
    set_quit_on_signal(SIGTERM);


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
    string indexdir = daemondir + '/' + f->first;
    IndexManager* index = f->second(indexdir.c_str());


    set<string> dirs = readdirstoindex(dirsfile);
    scheduler.setIndexedDirectories(dirs);
    scheduler.setIndexManager(index);
    
    EventListenerQueue listenerEventQueue;
    scheduler.setEventListenerQueue (&listenerEventQueue);

    // start the indexer thread
    scheduler.start();

#ifdef HAVE_INOTIFY
    // configure & start inotfy's watcher thread
    if (inotifyListener.init())
    {
        inotifyListener.setEventListenerQueue (&listenerEventQueue);
        inotifyListener.setIndexReader (index->getIndexReader());
        inotifyListener.setIndexedDirectories(dirs);
        inotifyListener.start();
    }
#endif
    
    // listen for requests
    Interface interface(*index, scheduler);
#ifdef HAVE_INOTIFY
    interface.setEventListener (&inotifyListener);
#endif

    SocketServer server(&interface);
    server.setSocketName(socketpath.c_str());
    if (!server.start()) {
        scheduler.stop();
#ifdef HAVE_INOTIFY
        inotifyListener.stop();
#endif
    }
    dirs = scheduler.getIndexedDirectories();
    savedirstoindex(dirsfile, dirs);

    // close the indexmanager
    delete index;

    // release lock
    releaseLock(lockfile, lock);
}


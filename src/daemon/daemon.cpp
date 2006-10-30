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
#include "interface.h"
#include "xsd/strigidaemonconfiguration.h"
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
#else
#include "pollinglistener.h"
#endif
#include "eventlistenerqueue.h"
#include "filtermanager.h"
#include "strigilogging.h"

#ifdef HAVE_DBUS
#include "dbusserver.h"
#endif
#include "socketserver.h"

#include <sstream>
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

vector<StrigiThread*> threads;

void
stopThreads() {
    vector<StrigiThread*>::const_iterator i;
    for (i=threads.begin(); i!=threads.end(); ++i) {
        STRIGI_LOG_INFO ("strigi.daemon", string("stopping thread") + (*i)->name);
        (*i)->stop();
        STRIGI_LOG_INFO ("strigi.daemon", "stopped another thread");
    }
}

void
quit_daemon(int) {
    STRIGI_LOG_INFO("strigi.daemon", "quit_daemon");
    static int interruptcount = 0;
    vector<StrigiThread*>::const_iterator i;
    switch (++interruptcount) {
    case 1:
        STRIGI_LOG_INFO ("strigi.daemon", "stopping threads");
        stopThreads();
        break;
    case 2:
        STRIGI_LOG_INFO ("strigi.daemon", "terminating threads");
        for (i=threads.begin(); i!=threads.end(); ++i) {
            (*i)->terminate();
        }
        break;
    case 3:
    default:
        STRIGI_LOG_FATAL ("strigi.daemon", "calling exit(1)")
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
/**
* Check if log4cxx file exists, otherwise creates a default one
**/
void
checkLogConf(const string& filename) {
    std::fstream confFile;
    confFile.open(filename.c_str(), std::ios::in);
    if (!confFile.is_open()){
        /*create the default configuration file*/
        confFile.open(filename.c_str(), std::ios::out);
        confFile << "# Set root logger level to DEBUG and its only appender to A1.\n";
        confFile << "log4j.rootLogger=DEBUG, A1\n\n";
        confFile << "# A1 is set to be a ConsoleAppender.\n";
        confFile << "log4j.appender.A1=org.apache.log4j.ConsoleAppender\n";
        confFile << "# A1 uses PatternLayout.\n";
        confFile << "log4j.appender.A1.layout=org.apache.log4j.PatternLayout\n";
        confFile << "log4j.appender.A1.layout.ConversionPattern=%d [%t] %-5p %c - %m%n\n";
    }
    confFile.close();
}
StrigiDaemonConfiguration
readStrigiConfiguration(const string& file) {
    std::stringbuf xml;
    std::ifstream f(file.c_str(), std::ios::binary);
    f.get(xml, '\0');
    f.close();
    StrigiDaemonConfiguration config(xml.str());
    if (xml.str().length() == 0) { // no config file: set default config
        config.a_useDBus = true;
        Repository r;
        string s;
        s = getenv("HOME"); 
        Path p;
        p.a_path = s;                 r.e_path.push_back(p);
        p.a_path = s + "/.kde";       r.e_path.push_back(p);
        p.a_path = s + "/.gnome2";    r.e_path.push_back(p);
        p.a_path = s + "/.evolution"; r.e_path.push_back(p);
        p.a_path = s + "/.mozilla";   r.e_path.push_back(p);
        p.a_path = s + "/.mozilla-thunderbird";   r.e_path.push_back(p);
        config.e_repository.push_back(r);
    }
    return config;
}
void
writeStrigiConfiguration(const string& file,
        const StrigiDaemonConfiguration& config) {
    ofstream f;
    f.open(file.c_str(), std::ios::binary);
    f << config;
    f.close();
}
/*set<string>
readdirstoindex(const string& file) {
    set<string> dirs;
    ifstream f(file.c_str());
    // if no file was available, use the HOME directory
    string s;
    if (!f.good()) {
        s = getenv("HOME"); 
        dirs.insert(s);
        dirs.insert(s+"/.kde");
        dirs.insert(s+"/.gnome2");
        dirs.insert(s+"/.evolution");
        dirs.insert(s+"/.mozilla");
        return dirs;
    }
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
}*/
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
    string conffile = daemondir+"/daemon.conf";
    string socketpath = daemondir+"/socket";
    string logconffile = daemondir+"/log.conf";
    string pathfilterfile = daemondir+"/pathfilter.conf";
    string patternfilterfile = daemondir+"/patternfilter.conf";

    // initialize the directory for the daemon data
    if (!initializeDir(daemondir)) {
        fprintf(stderr, "Could not initialize the daemon directory.\n");
        exit(1);
    }

    // init logging
    checkLogConf(logconffile);
    STRIGI_LOG_INIT(logconffile)

    // init filter manager
    FilterManager filterManager;
    filterManager.setConfFile (patternfilterfile, pathfilterfile);
    STRIGI_LOG_DEBUG("strigi.daemon", "filter manager initialized")
    
    IndexScheduler scheduler;
    scheduler.setFilterManager (&filterManager);

    if (!initializeDir(lucenedir)) {
        STRIGI_LOG_FATAL ("strigi.daemon", "Could not initialize the clucene directory.")
        exit(1);
    }
    if (!initializeDir(estraierdir)) {
        STRIGI_LOG_FATAL ("strigi.daemon", "Could not initialize the estraier directory.")
        exit(1);
    }

    // check that no other daemon is running
    struct flock lock;
    FILE* lockfile = aquireLock(lockfilename.c_str(), lock);
    if (lockfile == 0) {
        STRIGI_LOG_FATAL ("strigi.daemon", "Daemon cannot run: the file " + lockfilename + " is locked.")
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

    StrigiDaemonConfiguration config(readStrigiConfiguration(conffile));
    set<string> dirs;
    list<Repository>::const_iterator i;
    for (i = config.e_repository.begin(); i != config.e_repository.end(); ++i) {
        list<Path>::const_iterator j;
        for (j = i->e_path.begin(); j != i->e_path.end(); ++j) {
            dirs.insert(j->a_path);
        }
    }
    scheduler.setIndexManager(index);
    scheduler.setIndexedDirectories(dirs);

    EventListenerQueue listenerEventQueue;
    scheduler.setEventListenerQueue (&listenerEventQueue);

    // start the indexer thread
    threads.push_back(&scheduler);
    scheduler.start(20);

    Interface interface(*index, scheduler);

    EventListener* listener;
    
#ifdef HAVE_INOTIFY
    // listen for requests
    listener = new InotifyListener (dirs);
#else
    listener = new PollingListener(dirs);
#endif
    
    // configure & start inotfy's watcher thread
    if (listener->init()) {
        listener->setEventListenerQueue (&listenerEventQueue);
        listener->setFilterManager (&filterManager);
        listener->setIndexReader (index->getIndexReader());
        // do not start scanning until execution is in the thread!
        // inotifyListener.setIndexedDirectories(dirs);
        listener->start(20);
    }
    interface.setEventListener (listener);
    threads.push_back(listener);

    interface.setFilterManager (&filterManager);

#ifdef HAVE_DBUS
    DBusServer dbusserver(&interface);
    if (config.a_useDBus) {
        threads.push_back(&dbusserver);
        dbusserver.start();
    }
#endif

    SocketServer server(&interface);
    server.setSocketName(socketpath.c_str());

    if (!server.listen()) {
        stopThreads();
    }
    dirs = scheduler.getIndexedDirectories();
    set<string>::iterator j;
    for (j = dirs.begin(); j != dirs.end(); ++j) {
        bool found = false;
        for (i = config.e_repository.begin();
                !found && i != config.e_repository.end(); ++i) {
            list<Path>::const_iterator k;
            for (k = i->e_path.begin(); !found && k != i->e_path.end(); ++k) {
                found = k->a_path == *j;
            }
        }
        if (!found) {
            Path p;
            p.a_path = *j;
            config.e_repository.begin()->e_path.push_back(p);
        }
    }

    // close the indexmanager
    delete index;

    //delete listener
    delete listener;

    writeStrigiConfiguration(conffile, config);
    
    // release lock
    releaseLock(lockfile, lock);
}


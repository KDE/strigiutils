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
#include "strigiconfig.h"
#include "interface.h"
#include "daemonconfigurator.h"
#include "combinedindexmanager.h"

#include "indexscheduler.h"
#include "analyzerconfiguration.h"
#include "xesamlivesearch.h"
#include "queue/jobqueue.h"

#if defined (HAVE_INOTIFY)
#include "inotifylistener.h"
#else
#include "pollinglistener.h"
#include "eventlistener.h"
#endif
#include "eventlistenerqueue.h"
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
#include <stdlib.h>
#include <string.h>

using namespace Strigi;
using namespace std;

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
FILE*
aquireLock(const char* lockfile, struct flock& lock) {
    FILE* f = fopen(lockfile, "w");
    if (f == 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return 0;
    }
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    int r = fcntl(fileno(f), F_SETLK, &lock);
    if (r == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
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

    // init daemon configurator
    DaemonConfigurator config (conffile);

    // init filter manager
    AnalyzerConfiguration ic;
    config.loadFilteringRules(ic);
    STRIGI_LOG_DEBUG("strigi.daemon", "filter manager initialized")

    IndexScheduler scheduler;
    scheduler.setIndexerConfiguration(&ic);

    if (!initializeDir(lucenedir)) {
        STRIGI_LOG_FATAL ("strigi.daemon",
                          "Could not initialize the clucene directory.")
        exit(1);
    }
    if (!initializeDir(estraierdir)) {
        STRIGI_LOG_FATAL ("strigi.daemon",
                          "Could not initialize the estraier directory.")
        exit(1);
    }

    // check that no other daemon is running
    struct flock lock;
    FILE* lockfile = aquireLock(lockfilename.c_str(), lock);
    if (lockfile == 0) {
        STRIGI_LOG_FATAL ("strigi.daemon",
                          "Daemon cannot run: the file " +
                           lockfilename + " is locked.")
        exit(1);
    }

    set<string> dirs = config.getIndexedDirectories();

    CombinedIndexManager* index = new CombinedIndexManager(
        config.getWriteableIndexType(), config.getWriteableIndexDir());
    list<Repository> rors = config.getReadOnlyRepositories();
    list<Repository>::const_iterator i;
    for (i = rors.begin(); i != rors.end(); ++i) {
        index->addReadonlyIndex(i->a_indexdir, i->a_type);
    }

    scheduler.setIndexManager(index);
    scheduler.setIndexedDirectories(dirs);

    EventListenerQueue listenerEventQueue;
    scheduler.setEventListenerQueue (&listenerEventQueue);

    // start the indexer thread
//    threads.push_back(&scheduler);
    scheduler.start(20);

    JobQueue queue(10);
    Interface interface(*index, scheduler);
    XesamLiveSearch xesam(index, queue);

    EventListener* listener = NULL;

#if defined (HAVE_INOTIFY)
    // listen for requests
    listener = new InotifyListener(dirs);
#else
    if (config.getPollingInterval() > 0) {
        listener = new PollingListener(dirs);
    } else {
        listener = 0;
    }
#endif

    // configure & start inotfy's watcher thread
    if (listener  && listener->init()) {
        listener->setEventListenerQueue (&listenerEventQueue);
        listener->setIndexerConfiguration(&ic);
        listener->setIndexReader (index->indexReader());
        listener->setPollingInterval (config.getPollingInterval());
        // do not start scanning until execution is in the thread!
        // inotifyListener.setIndexedDirectories(dirs);
        listener->start(20);
    }
    interface.setEventListener (listener);
//    threads.push_back(listener);

#ifdef HAVE_DBUS
    DBusServer dbusserver(&interface, &xesam);
    if (config.useDBus()) {
        dbusserver.start();
    }
#endif

    SocketServer server(&interface);
    server.setSocketName(socketpath.c_str());

    server.listen();
    StrigiThread::stopThreads();

    //save indexed dirs
    dirs = scheduler.getIndexedDirectories();
    config.setIndexedDirectories (dirs);

    //save the pollingtime
    if (listener) {
        config.setPollingInterval (listener->getPollingInterval());
    }

    //save filtering rules
    config.saveFilteringRules(scheduler.getIndexerConfiguration().filters());

    //save the updated xml configuration file
    config.save();

    // close the indexmanager
    delete index;

    //delete listener
    delete listener;

    // release lock
    releaseLock(lockfile, lock);
}


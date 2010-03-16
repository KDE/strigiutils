/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006-2007 Jos van den Oever <jos@vandenoever.info>
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
#include <strigi/strigiconfig.h>
#include "interface.h"
#include "daemonconfigurator.h"
#include "combinedindexmanager.h"
#include <strigi/indexpluginloader.h>

#include "indexscheduler.h"
#include <strigi/analyzerconfiguration.h>
#include "xesam/xesamlivesearch.h"
#include "queue/jobqueue.h"

#if defined (HAVE_FAM)
#include "famlistener.h"
#elif defined (HAVE_INOTIFY)
#include "inotifylistener.h"
#else
#include "eventlistener/pollinglistener.h"
#include "eventlistener/eventlistener.h"
#endif
#include "eventlistener/eventlistenerqueue.h"
#include "strigilogging.h"

#ifdef HAVE_DBUS
#include "dbus/dbusserver.h"
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
#include <algorithm>

using namespace Strigi;
using namespace std;

namespace {
    // The default directory from which Strigi reads the configuration file
    // and where it stores the index can be set via the command line.
    string userForcedStrigiDir;
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
#ifdef _WIN32
            r = mkdir(dir.c_str());
#else
            r = mkdir(dir.c_str(), 0700);
#endif
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
void
printVersion(int argc, char** argv) {
    printf( "%s %s\n Copyright (C) 2006-2007 Jos van den Oever and others\n",
            argv[0],
            STRIGI_VERSION_STRING );
}
void
printUsage(int argc, char** argv) {
    printf( "%s [--version] [--help] [-d <strigidir>]\n",
            argv[0] );
    printf( "  --version      Display the version.\n" );
    printf( "  --help         Display this help text.\n" );
    printf( "  -d <strigidir> Override the default strigi directory (~/.strigi).\n" );
}
void
printBackendList() {
    std::vector<std::string> backends = IndexPluginLoader::indexNames();
    for ( unsigned int i = 0; i < backends.size(); ++i ) {
        printf( "%s\n", backends[i].c_str() );
    }
}
void
checkArgs(int argc, char** argv) {
    int i = 1;
    while ( i < argc ) {
        if ( !strcmp( argv[i], "--version" ) ) {
            printVersion(argc, argv);
            exit( 0 );
        }
        else if ( !strcmp( argv[i], "--help" ) ) {
            printUsage(argc, argv);
            exit( 0 );
        }
        else if ( !strcmp( argv[i], "-d" ) ) {
            ++i;
            if ( i >= argc ) {
                printUsage(argc, argv);
                exit( 1 );
            }
            userForcedStrigiDir = argv[i];
        }

        ++i;
    }
}
void
ensureBackend( const std::string& backendName ) {
    std::vector<std::string> backends = IndexPluginLoader::indexNames();
    if ( std::find( backends.begin(), backends.end(), backendName ) == backends.end() ) {
        fprintf( stderr, "Unknown backend type: %s\n", backendName.c_str() );
        exit( 2 );
    }
}
int
main(int argc, char** argv) {
    checkArgs( argc,argv );

    // set up the directory paths
    string daemondir;
    if (userForcedStrigiDir.length() == 0) {
        char* home = getenv("HOME");
        if (home == NULL) {
            cerr << "Environment variable 'HOME' must be defined." << endl;
            exit(1);
        }
        string homedir(home);
        daemondir = homedir+"/.strigi";
    } else {
        daemondir = userForcedStrigiDir;
    }
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
    StoppableConfiguration ic;
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

    // before creating the indexer, check if the index backend exists
    ensureBackend( config.getWriteableIndexType() );

    CombinedIndexManager* index = new CombinedIndexManager(
        config.getWriteableIndexType(),
        config.getWriteableIndexDir());
    list<Repository> rors = config.getReadOnlyRepositories();
    list<Repository>::const_iterator i;
    for (i = rors.begin(); i != rors.end(); ++i) {
        index->addReadonlyIndex(i->a_indexdir, i->a_type);
    }

    scheduler.setIndexManager(index);
    scheduler.setIndexedDirectories(dirs);

    EventListenerQueue* listenerEventQueue = new EventListenerQueue();
    listenerEventQueue->start();
    scheduler.setEventListenerQueue (listenerEventQueue);

    // start the indexer thread
//    threads.push_back(&scheduler);
    // start with low priority
    scheduler.start(20);

    JobQueue queue(1);
    Interface interface(*index, scheduler);
    XesamLiveSearch xesam(index, queue);

    EventListener* listener = NULL;
#if defined (HAVE_FAM)
    listener = new FamListener (dirs);
#elif defined (HAVE_INOTIFY)
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
        listener->setEventListenerQueue (listenerEventQueue);
        listener->setIndexerConfiguration(&ic);
        listener->setCombinedIndexManager (index);
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
    // stop the listener threads
    StrigiThread::stopThreads();

    // stop the queue, so it will free memory and stop accepting new jobs
    queue.stop();

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

    //delete listener event queue
    delete listenerEventQueue;

    // release lock
    releaseLock(lockfile, lock);
}


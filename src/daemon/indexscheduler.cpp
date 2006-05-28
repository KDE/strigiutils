#include "indexscheduler.h"
#include "indexmanager.h"
#include "indexreader.h"
#include "indexwriter.h"
#include "filelister.h"
#include "streamindexer.h"
#include <cerrno>
#include <sys/resource.h>
using namespace std;
using namespace jstreams;

IndexScheduler* sched;

pthread_mutex_t IndexScheduler::initlock = PTHREAD_MUTEX_INITIALIZER;

IndexScheduler::IndexScheduler() {
    sched = this;
    state = Idling;
}
IndexScheduler::~IndexScheduler() {
}
void*
indexschedulerstart(void *d) {
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

    // start the actual work
    static_cast<IndexScheduler*>(d)->run(0);
    return 0;
}
bool
IndexScheduler::addFileCallback(const string& path, const char *filename,
        time_t mtime) {
    if (sched->state != Indexing) return false;
    // only read files that do not start with '.'
    if (path.find("/.") != string::npos || *filename == '.') return true;

    std::string filepath(path+filename);

    map<string, time_t>::iterator i = sched->dbfiles.find(filepath);
    if (i == sched->dbfiles.end() || i->second != mtime) {
        sched->toindex[filepath] = mtime;
    } else {
        sched->dbfiles.erase(i);
    }
    return true;
}
int
IndexScheduler::start() {
    // start the indexer thread
    int r = pthread_create(&thread, NULL, indexschedulerstart, this);
    if (r < 0) {
        printf("cannot create thread\n");
        return 1;
    }
    return 0;
}
void
IndexScheduler::stop() {
    state = Stopping;
    if (thread) {
        // wait for the indexer to finish
        pthread_join(thread, 0);
    }
    thread = 0;
}
void
IndexScheduler::terminate() {
    // TODO
}
std::string
IndexScheduler::getState() {
    if (state == Idling) return "idling";
    if (state == Indexing) return "indexing";
    return "stopping";
}
int
IndexScheduler::getQueueSize() {
    return toindex.size();
}
void
shortsleep(long nanoseconds) {
    // set sleep time
    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = nanoseconds;
    nanosleep(&sleeptime, 0);
}
void *
IndexScheduler::run(void*) {
    while (state != Stopping) {
        shortsleep(100000000);
        if (state == Indexing) {
            index();
            if (state == Indexing) {
                state = Idling;
            }
        }
    }
    return 0;
}
void
IndexScheduler::index() {
    IndexReader* reader = indexmanager->getIndexReader();
    IndexWriter* writer = indexmanager->getIndexWriter();
    StreamIndexer* streamindexer = new StreamIndexer(writer);


    if (dbfiles.size() == 0 && toindex.size() == 0) {
        // retrieve the list of real files currently in the database
        dbfiles = reader->getFiles(0);
        printf("%i real files in the database\n", dbfiles.size()); 

        // first loop through all files
        FileLister lister;
        lister.setCallbackFunction(&addFileCallback);
        printf("going to index\n");
        set<string>::const_iterator i;
        for (i = dirstoindex.begin(); i != dirstoindex.end(); ++i) {
            lister.listFiles(i->c_str());
        }
        printf("%i files to remove\n", dbfiles.size()); 
        printf("%i files to add or update\n", toindex.size());
    }

    map<string,time_t>::iterator it = dbfiles.begin();
    while (state == Indexing && it != dbfiles.end()) {
        writer->deleteEntry(it->first);
        dbfiles.erase(it++);
    }

    it = toindex.begin();
    while (state == Indexing && it != toindex.end()) {
        streamindexer->indexFile(it->first);
        if (writer->itemsInCache() > 10000) {
            writer->commit();
        }
        toindex.erase(it++);
    }
    writer->commit();

    delete streamindexer;
}
void
IndexScheduler::setIndexedDirectories(const std::set<std::string> &d) {
    dirstoindex.clear();
    std::set<std::string>::const_iterator i;
    for (i = d.begin(); i!=d.end(); ++i) {
        bool ok = true;
        std::set<std::string>::iterator j;
        for (j = dirstoindex.begin(); ok && j != dirstoindex.end(); ++j) {
            if (j->length() >= i->length()
                && j->substr(0, i->length()) == *i) {
                dirstoindex.erase(j);
                j = dirstoindex.begin();
            } else if (i->length() >= j->length()
                && i->substr(0, j->length()) == *j) {
                ok = false;
            }
        }
        if (ok) {
            string dir = *i;
            if (dir[dir.length()-1] == '/') {
                dir = dir.substr(0, dir.length()-1);
            }
            dirstoindex.insert(dir);
        }
    }
}

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
    keeprunning = true;
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
IndexScheduler::addFileCallback(const string& path, const char *filename, time_t mtime) {
    if (!sched->keeprunning) return false;
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
    keeprunning = false;
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
int
IndexScheduler::getQueueSize() {
    return toindex.size();
}
void *
IndexScheduler::run(void*) {
    IndexWriter* writer = indexmanager->getIndexWriter();
    StreamIndexer* streamindexer = new StreamIndexer(writer);

    IndexReader* reader = indexmanager->getIndexReader();

    dbfiles = reader->getFiles(0);
    printf("%i real files in the database\n", dbfiles.size()); 

    // first loop through all files
    FileLister lister;
    lister.setCallbackFunction(&addFileCallback);
    printf("going to index\n");
    lister.listFiles(dirtoindex.c_str());
    printf("%i files to remove\n", dbfiles.size()); 
    printf("%i files to add or update\n", toindex.size()); 

    map<string,time_t>::iterator it = dbfiles.begin();
    while (keeprunning && it != dbfiles.end()) {
        writer->deleteEntry(it->first);
        dbfiles.erase(it++);
    }

    it = toindex.begin();
    while (keeprunning && it != toindex.end()) {
        streamindexer->indexFile(it->first);
        if (writer->itemsInCache() > 10000) {
            writer->commit();
        }
        toindex.erase(it++);
    }
    writer->commit();

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
    return 0;
}

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
#include "indexscheduler.h"
#include "indexmanager.h"
#include "indexreader.h"
#include "indexwriter.h"

#include "event.h"
#include "eventlistenerqueue.h"
#include "filtermanager.h"

#include "filelister.h"
#include "streamindexer.h"
#include <cerrno>
#include <sys/resource.h>

#include "strigilogging.h"

using namespace std;
using namespace jstreams;

IndexScheduler* sched;

pthread_mutex_t IndexScheduler::initlock = PTHREAD_MUTEX_INITIALIZER;

IndexScheduler::IndexScheduler() {
    sched = this;
    state = Idling;
    m_listenerEventQueue = NULL;
    m_filterManager = NULL;
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
        if (r==-1)
            STRIGI_LOG_ERROR ("strigi.IndexScheduler.indexschedulerstart", string("error setting priority: ") + strerror(errno))
        //nice(20);
    }
#ifdef HAVE_LINUXIOPRIO
    sys_ioprio_set(IOPRIO_WHO_PROCESS, 0, IOPRIO_CLASS_IDLE);
#endif

    // start the actual work
    static_cast<IndexScheduler*>(d)->run(0);
    pthread_exit(0);
}
bool
IndexScheduler::addFileCallback(const char* path, uint dirlen, uint len,
        time_t mtime) {
    if (sched->state != Indexing) return false;
    // only read files that do not start with '.'
    if (strstr(path, "/.")) return true;
    // check filtering rules given by user
    if (sched->m_filterManager == NULL)
    {
        STRIGI_LOG_WARNING ("strigi.IndexScheduler.addFileCallback", "unable to use filters, m_filterManager == NULL!")
    }
    else if ((sched->m_filterManager)->findMatch (path))
    {
        STRIGI_LOG_WARNING ("strigi.IndexScheduler.indexFileCallback", "ignoring file " + string(path))
        return true;
    }
    
    std::string filepath(path, len);

    map<string, time_t>::iterator i = sched->dbfiles.find(filepath);
    // if the file has not yet been indexed or if the mtime has changed,
    // put it in the list to index
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
        STRIGI_LOG_ERROR ("strigi.IndexScheduler", "cannot create thread")
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
        else if (state == Idling)
        {
            if (m_listenerEventQueue == NULL)
                return 0;
            
            vector <Event*> events = m_listenerEventQueue->getEvents();
            if (events.size() > 0)
            {
                state = Indexing;
                processListenerEvents(events);
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
        
        char buff [20];
        snprintf(buff, 20* sizeof (char), "%i", dbfiles.size());
        STRIGI_LOG_DEBUG ("strigi.IndexScheduler", string(buff) + " real files in the database") 
        
        // first loop through all files
        FileLister lister;
        lister.setCallbackFunction(&addFileCallback);
        STRIGI_LOG_DEBUG ("strigi.IndexScheduler", "going to index")
        set<string>::const_iterator i;
        for (i = dirstoindex.begin(); i != dirstoindex.end(); ++i) {
            lister.listFiles(i->c_str());
        }
        
        snprintf(buff, 20* sizeof (char), "%i", dbfiles.size());
        STRIGI_LOG_DEBUG ("strigi.IndexScheduler", string(buff) + " files to remove")
        
        snprintf(buff, 20* sizeof (char), "%i", toindex.size());
        STRIGI_LOG_DEBUG ("strigi.IndexScheduler", string(buff) + " files to add or update")
    }

    vector<string> todelete;
    map<string,time_t>::iterator it = dbfiles.begin();
    while (state == Indexing && it != dbfiles.end()) {
        todelete.push_back(it->first);
//        writer->deleteEntry(it->first);
        dbfiles.erase(it++);
    }
    writer->deleteEntries(todelete);

    it = toindex.begin();
    while (state == Indexing && it != toindex.end()) {
        streamindexer->indexFile(it->first);
        if (writer->itemsInCache() > 10000) {
            writer->commit();
        }
        toindex.erase(it++);
    }
    if (state == Indexing) {
        writer->commit();
        writer->optimize();
    }

    delete streamindexer;
}

void IndexScheduler::processListenerEvents(vector<Event*>& events)
{
    IndexReader* reader = indexmanager->getIndexReader();
    IndexWriter* writer = indexmanager->getIndexWriter();
    StreamIndexer* streamindexer = new StreamIndexer(writer);
    
    vector<string> toDelete, toIndex;
    
    STRIGI_LOG_DEBUG ("strigi.IndexScheduler", "processing listener's events")

    for (vector<Event*>::iterator iter = events.begin(); iter != events.end(); iter++)
    {
        Event* event = *iter;
        switch (event->getType())
        {
            case Event::CREATED:
                toIndex.push_back (event->getPath());
                break;
            case Event::UPDATED:
                toIndex.push_back (event->getPath());
                toDelete.push_back (event->getPath());
                break;
            case Event::DELETED:
                toDelete.push_back (event->getPath());
                break;
        }
        
        STRIGI_LOG_DEBUG ("strigi.IndexScheduler", "event infos: " + event->toString())
        
        delete event;
    }
    writer->deleteEntries(toDelete);

    for (unsigned int i = 0; i < toIndex.size(); i++)
    {
        streamindexer->indexFile(toIndex[i]);
        if (writer->itemsInCache() > 10000) {
            writer->commit();
        }
    }
    
    writer->commit();
    writer->optimize();
    
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

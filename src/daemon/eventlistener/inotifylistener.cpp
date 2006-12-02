/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Flavio Castelli <flavio.castelli@gmail.com>
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
#include "inotifylistener.h"
#include "pollinglistener.h"

#include "event.h"
#include "eventlistenerqueue.h"
#include "filelister.h"
#include "indexreader.h"
#include "../strigilogging.h"

#include <cerrno>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/types.h>
#include <vector>

#include "local_inotify.h"
#include "local_inotify-syscalls.h"

InotifyListener* InotifyListener::workingInotifyListener;
InotifyListener::ReindexDirsThread* InotifyListener::ReindexDirsThread::workingReindex;

using namespace std;
using namespace jstreams;

InotifyListener::InotifyListener(set<string>& indexedDirs)
    :EventListener("InotifyListener")
{
    workingInotifyListener = this;

    // listen only to interesting events
    m_iEvents = IN_CLOSE_WRITE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO
        | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF;

    m_bMonitor = true;
    setState(Idling);
    m_bInitialized = false;
    m_pollingListener = NULL;
    m_reindexDirThread = NULL;
    STRIGI_MUTEX_INIT (&m_watchesLock);

    // fix path, all dir must end with a '/'
    for (set<string>::iterator iter = indexedDirs.begin(); iter != indexedDirs.end(); iter++)
        m_indexedDirs.insert (fixPath(*iter));
}

InotifyListener::~InotifyListener()
{
    clearWatches();

    if (m_pollingListener != NULL)
    {
        m_pollingListener->stop();
        delete m_pollingListener;
        m_pollingListener = NULL;
    }

    if (m_reindexDirThread != NULL)
    {
        m_reindexDirThread->stop();
        delete m_reindexDirThread;
        m_reindexDirThread = NULL;
    }

    STRIGI_MUTEX_DESTROY (&m_watchesLock);
}

bool InotifyListener::init()
{
    m_iInotifyFD = inotify_init();
    if (m_iInotifyFD < 0)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener", "inotify_init() failed.  Are you running Linux 2.6.13 or later? If so, something mysterious has gone wrong.")
        return false;
    }

    m_bInitialized = true;

    STRIGI_LOG_DEBUG ("strigi.InotifyListener", "successfully initialized")

    return true;
}

void* InotifyListener::run(void*)
{
    STRIGI_LOG_DEBUG ("strigi.InotifyListener.run", "started bootstrap procedure");
    bootstrap (m_indexedDirs);
    STRIGI_LOG_DEBUG ("strigi.InotifyListener.run", "finished bootstrap procedure");

    while (getState() != Stopping)
    {
        watch();

        // check if ReindexDirsThread is running and has finished his jobs
        if ((m_reindexDirThread != NULL) && (m_reindexDirThread->m_bfinished))
        {
            STRIGI_MUTEX_LOCK (&m_reindexDirThread->m_resourcesLock);

            // remove old indexed files from db
            dirsRemoved (m_reindexDirThread->m_nomoreIndexedDirs, m_reindexDirThread->m_events);

            // add new watches
            addWatches (m_reindexDirThread->m_toWatch);

            if (m_reindexDirThread->m_events.size() > 0)
            {
                m_pEventQueue->addEvents (m_reindexDirThread->m_events);
                m_reindexDirThread->m_events.clear();
            }

            m_indexedDirs = m_reindexDirThread->m_newDirs;

            STRIGI_MUTEX_UNLOCK (&m_reindexDirThread->m_resourcesLock);
        }

        if (getState() == Working)
            setState(Idling);
    }

    STRIGI_LOG_DEBUG ("strigi.InotifyListener.run", string("exit state: ") + getStringState());
    return 0;
}

void InotifyListener::watch ()
{
    if (m_pEventQueue == NULL)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.watch", "m_pEventQueue == NULL!")
        return;
    }

    // some code taken from inotify-tools (http://inotify-tools.sourceforge.net/)

    vector <Event*> events;

    struct timeval read_timeout;
    read_timeout.tv_sec = 1;
    read_timeout.tv_usec = 0;

    static const int MAX_EVENTS = 4096;
    struct inotify_event event[MAX_EVENTS];
    event[0].wd = 0;
    event[0].mask = 0;
    event[0].cookie = 0;
    event[0].len = 0;

    size_t bytes = 0;
    fd_set read_fds;

    bytes = 0;

    while ( bytes < sizeof(struct inotify_event) )
    {

        FD_ZERO(&read_fds);
        FD_SET(m_iInotifyFD, &read_fds);

        int rc = select(m_iInotifyFD + 1, &read_fds, NULL, NULL, &read_timeout);

        if ( rc < 0 )
        {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.watch", "Select on inotify failed")
            return;
        }
        else if ( rc == 0 )
        {
            //Inotify select timeout
            return;
        }

        int thisBytes = read(m_iInotifyFD, &event + bytes, sizeof(struct inotify_event)*MAX_EVENTS - bytes);

        if ( thisBytes < 0 )
        {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.watch", "Read from inotify failed")
            return;
        }

        if ( thisBytes == 0 )
        {
            STRIGI_LOG_WARNING ("strigi.InotifyListener.watch", "Inotify reported end-of-file.  Possibly too many events occurred at once.")
            return;
        }

        bytes += thisBytes;
    }

    static struct inotify_event * this_event;

    static char * this_event_char;
    this_event_char = (char *)&event[0];

    static uint remaining_bytes;
    remaining_bytes = bytes;

    do
    {
        string message;
        string watchName;
        int watchID;

        this_event = (struct inotify_event *)this_event_char;

        STRIGI_MUTEX_LOCK (&m_watchesLock);

        map <int, string>::iterator watchIter = m_watches.find (this_event->wd);

        if (watchIter == m_watches.end())
        {
            if ((this_event->wd && IN_IGNORED) == 0)
            {
                // event wasn't marked with IGNORE flag, print some message

                char buff [20];
                snprintf(buff, 20 * sizeof (char), "%i",this_event->wd);

                STRIGI_LOG_WARNING ("strigi.InotifyListener.watch", string("returned an unknown watch descriptor: ") + buff)

                string eventtype = eventToString (this_event->mask);

                STRIGI_LOG_WARNING ("strigi.InotifyListener.watch", "missed event type: " + eventtype)
            }
            // jump to next event
            this_event_char += sizeof(struct inotify_event) + this_event->len;
            remaining_bytes -= sizeof(struct inotify_event) + this_event->len;

            STRIGI_MUTEX_UNLOCK (&m_watchesLock);

            continue;
        }

        watchName = watchIter->second;
        watchID   = watchIter->first;

        STRIGI_MUTEX_UNLOCK (&m_watchesLock);

        if (isEventInteresting (this_event))
        {
            STRIGI_LOG_DEBUG ("strigi.InotifyListener.watch", watchName + " changed")
                STRIGI_LOG_DEBUG ("strigi.InotifyListener.watch", string("caught inotify event: ") + eventToString( this_event->mask))

            if ((this_event->len > 0))
                STRIGI_LOG_DEBUG ("strigi.InotifyListener.watch", "event regards " + string(this_event->name, this_event->len))

            string file (watchName + string(this_event->name));

            if ( (IN_MODIFY & this_event->mask) != 0 )
            {
                Event* event = new Event (Event::UPDATED, file);
                events.push_back (event);
            }
            else if (( (IN_DELETE & this_event->mask) != 0 ) || ( (IN_MOVED_FROM & this_event->mask) != 0 ) || ( (IN_DELETE_SELF & this_event->mask) != 0 ) || ( (IN_MOVE_SELF & this_event->mask) != 0 ))
            {
                if ((IN_ISDIR & this_event->mask) != 0)
                    dirRemoved (file, events);
                else
                {
                    Event* event = new Event (Event::DELETED, file);
                    events.push_back (event);
                }
            }
            else if ( (IN_CLOSE_WRITE & this_event->mask) != 0 )
            {
                Event* event = new Event (Event::UPDATED, file);
                events.push_back (event);
            }
            else if (( (IN_CREATE & this_event->mask) != 0 ) || ( (IN_MOVED_TO & this_event->mask) != 0 ))
            {
                if ( (IN_ISDIR & this_event->mask) != 0 )
                {
                    // a new directory has been created / moved into a watched place
                    m_toIndex.clear();
                    m_toWatch.clear();

                    FileLister lister();

                    lister.setFileCallbackFunction(&indexFileCallback);
                    lister.setDirCallbackFunction(&watchDirCallback);

                    lister.listFiles(file.c_str());

                    for (map<string,time_t>::iterator i = m_toIndex.begin(); i != m_toIndex.end(); i++)
                    {
                        Event* event = new Event (Event::CREATED, i->first);
                        events.push_back (event);
                    }

                    // add new watches
                    addWatches (m_toWatch);

                    m_toWatch.clear();
                    m_toIndex.clear();
                }
                else
                {
                    Event* event = new Event (Event::CREATED, file);
                    events.push_back (event);
                }
            }
            else
                STRIGI_LOG_DEBUG ("strigi.InotifyListener.watch", "inotify's unknown event")
        }

        this_event_char += sizeof(struct inotify_event) + this_event->len;
        remaining_bytes -= sizeof(struct inotify_event) + this_event->len;
    }
    while (remaining_bytes >= sizeof(struct inotify_event) );

    // I _think_ this should never happen.
    if (remaining_bytes != 0 ) {
        string message;
        char buff [20];

        snprintf(buff, 20 * sizeof (char), "%f", (((float)remaining_bytes)/((float)sizeof(struct inotify_event))));
        message = buff;
        message += "event(s) may have been lost!";
        STRIGI_LOG_ERROR ("strigi.InotifyListener.watch", message)
    }

    if (events.size() > 0)
        m_pEventQueue->addEvents (events);

    fflush( NULL );
}

bool
InotifyListener::isEventInteresting (struct inotify_event * event)
{
    // ignore files starting with '.'
    if (((IN_ISDIR & event->mask) == 0) && (event->len > 0) && ((event->name)[0] == '.'))
        return false;

    if (m_pFilterManager != NULL)
    {
        if ((event->len > 0) && m_pFilterManager->findMatch(event->name, event->len))
            return false;
    }
    else
        STRIGI_LOG_WARNING ("strigi.InotifyListener.isEventInteresting", "unable to use filters, m_pFilterManager == NULL!")

    return true;
}

bool InotifyListener::addWatch (const string& path)
{
    if (!m_bInitialized)
        return false;

    STRIGI_MUTEX_LOCK (&m_watchesLock);

    map<int, string>::iterator iter;
    for (iter = m_watches.begin(); iter != m_watches.end(); iter++)
    {
        if ((iter->second).compare (path) == 0) // dir is already watched
        {
            STRIGI_MUTEX_UNLOCK (&m_watchesLock);
            return true;
        }
    }

    static int wd;
    wd = inotify_add_watch (m_iInotifyFD, path.c_str(), m_iEvents);

    if (wd < 0)
    {
        STRIGI_MUTEX_UNLOCK (&m_watchesLock);

        if ((wd == -1) && ( errno == ENOSPC))
        {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.addWatch", "Failed to watch, maximum watch number reached")
            STRIGI_LOG_ERROR ("strigi.InotifyListener.addWatch", "You've to increase the value stored into /proc/sys/fs/inotify/max_user_watches")
        }
        else if ( wd == -1 )
            STRIGI_LOG_ERROR ("strigi.InotifyListener.addWatch", "Failed to watch " + path + " because of: " + strerror(-wd))
        else
        {
            char buff [20];
            snprintf(buff, 20* sizeof (char), "%i", wd);

            STRIGI_LOG_ERROR ("strigi.InotifyListener.addWatch", "Failed to watch " + path + ": returned wd was " + buff + " (expected -1 or >0 )")
        }

        return false;
    }
    else
    {
        m_watches.insert(make_pair(wd, path));

        STRIGI_MUTEX_UNLOCK (&m_watchesLock);

        STRIGI_LOG_INFO ("strigi.InotifyListener.addWatch", "added watch for " + path)

        return true;
    }
}

void InotifyListener::addWatches (const set<string> &watches)
{
    set<string>::iterator iter;
    set<string> toPool;

    for (iter = watches.begin(); iter != watches.end(); iter++)
    {
        if (!addWatch (*iter))
        {

            if (errno == ENOSPC)
                 // user can't add no more watches, it's useless to go on
                break;

            // adding watch failed for other reason, keep trying with others
            toPool.insert(*iter);
        }
    }

    if (iter != watches.end())
    {
        // probably we reached the max_user_watches limit
        for ( ; iter != watches.end(); iter++)
            toPool.insert (*iter);
    }

    if (!toPool.empty())
    {
        if (m_pollingListener == NULL)
        {
            m_pollingListener = new PollingListener();
            m_pollingListener->setEventListenerQueue( m_pEventQueue);
            m_pollingListener->setIndexReader( m_pIndexReader);
            m_pollingListener->setFilterManager( m_pFilterManager);
            //TODO: start with a low priority?
            m_pollingListener->start( );
        }

        m_pollingListener->addWatches( toPool);
    }
}

void InotifyListener::rmWatch(int wd, string path)
{
    char buff [20];
    snprintf(buff, 20 * sizeof (char), "%i",wd);

    if (inotify_rm_watch (m_iInotifyFD, wd) == -1)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.rmWatch", string("Error removing watch ") + buff + " associated to path: " + path)
        STRIGI_LOG_ERROR ("strigi.InotifyListener.rmWatch", string("error: ") + strerror(errno))
    }
    else
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.rmWatch", string("Removed watch ") + buff + " associated to path: " + path)
}

void InotifyListener::clearWatches ()
{
    STRIGI_MUTEX_LOCK (&m_watchesLock);

    for (map<int, string>::iterator iter = m_watches.begin(); iter != m_watches.end(); iter++)
        rmWatch (iter->first, iter->second);

    m_watches.clear();

    STRIGI_MUTEX_UNLOCK (&m_watchesLock);
}

void InotifyListener::dirRemoved (string dir, vector<Event*>& events)
{
    dir = fixPath (dir);

    STRIGI_LOG_DEBUG ("strigi.InotifyListener.dirRemoved", dir + " is no longer watched, removing all indexed files contained")

    // we've to de-index all files contained into the deleted/moved directory
    if (m_pIndexReader) {
        // all indexed files
        map<string, time_t> indexedFiles = m_pIndexReader->getFiles( 0);

        // remove all entries that were contained into the removed dir
        for (map<string, time_t>::iterator it = indexedFiles.begin(); it != indexedFiles.end(); it++)
        {
            string::size_type pos = (it->first).find (dir);
            if (pos == 0)
            {
                Event* event = new Event (Event::DELETED, it->first);
                events.push_back (event);
            }
        }
    }
    else
        STRIGI_LOG_WARNING ("strigi.InotifyListener.dirRemoved", "m_pIndexReader == NULL!")

    // remove inotify watches over no more indexed dirs
    STRIGI_MUTEX_LOCK (&m_watchesLock);

    map<int, string>::iterator mi = m_watches.begin();
    while ( mi != m_watches.end())
    {
        if ((mi->second).find (dir,0) == 0)
        {
            // directory name begins with param dir --> it's a subfolder of dir
            map<int, string>::iterator rmIt = mi;
            mi++;
            m_watches.erase (rmIt);
        }
        else
            mi++;
    }

    STRIGI_MUTEX_UNLOCK (&m_watchesLock);
}

void InotifyListener::dirsRemoved (set<string> dirs, vector<Event*>& events)
{
    if (m_pIndexReader) {
        // all indexed files
        map<string, time_t> indexedFiles = m_pIndexReader->getFiles( 0);

        // remove all entries that were contained into the removed dirs
        for (map<string, time_t>::iterator fileIt = indexedFiles.begin(); fileIt != indexedFiles.end(); fileIt++)
        {
            for (set<string>::iterator dirIt = dirs.begin(); dirIt != dirs.end(); dirIt++)
            {
                // ensure all dirs terminate with '/' (*nix) or '\' (windows)
                string dir (fixPath (*dirIt));

                string::size_type pos = (fileIt->first).find (dir);
                if (pos == 0)
                {
                    Event* event = new Event (Event::DELETED, fileIt->first);
                    events.push_back (event);
                    break;
                }
            }
        }
    }
    else
        STRIGI_LOG_WARNING ("strigi.InotifyListener.dirsRemoved", "m_pIndexReader == NULL!")

    // remove inotify watches over no more indexed dirs
    for (set<string>::iterator it = dirs.begin(); it != dirs.end(); it++)
    {
        // ensure all dirs terminate with '/' (*nix) or '\' (windows)
        string dir (fixPath (*it));

        STRIGI_MUTEX_LOCK (&m_watchesLock);

        map<int, string>::iterator mi = m_watches.begin();
        while ( mi != m_watches.end())
        {
            if ((mi->second).find(dir) == 0)
            {
                // directory name begins with it
                map<int, string>::iterator rmIt = mi;
                mi++;
                m_watches.erase (rmIt);
            }
            else
                mi++;
        }

        STRIGI_MUTEX_UNLOCK (&m_watchesLock);
    }
}

void InotifyListener::setIndexedDirectories (const set<string> &dirs) {
    if (!m_pIndexReader) {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.setIndexedDirectories", "m_pIndexReader == NULL!")
        return;
    }
    if (m_pEventQueue == NULL)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.setIndexedDirectories", "m_pEventQueue == NULL!")
        return;
    }

    set<string> fixedDirs;

    // fix path, all dir must end with a '/'
    for (set<string>::iterator iter = dirs.begin(); iter != dirs.end(); iter++)
        fixedDirs.insert (fixPath (*iter));

    if (m_reindexDirThread == NULL)
    {
        m_reindexDirThread = new ReindexDirsThread ("reindexDirThread", fixedDirs, m_indexedDirs);
        m_reindexDirThread->setFilterManager( m_pFilterManager);
        m_reindexDirThread->setIndexReader( m_pIndexReader);

        m_reindexDirThread->start();
    }
    else
        m_reindexDirThread->setIndexedDirs( fixedDirs);

    string newdirs ("|");

    for (set<string>::iterator iter = fixedDirs.begin(); iter != fixedDirs.end(); iter++)
        newdirs += (*iter + "|");

    STRIGI_LOG_DEBUG ("strigi.InotifyListener.setIndexedDirectories", "new indexed dirs: " + newdirs)
}

void InotifyListener::bootstrap (const set<string> &dirs) {
    if (!m_pIndexReader) {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.bootstrap", "m_pIndexReader == NULL!")
        return;
    }
    if (m_pEventQueue == NULL)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.bootstrap", "m_pEventQueue == NULL!")
        return;
    }

    vector<Event*> events;
    map <string, time_t> indexedFiles = m_pIndexReader->getFiles(0);

    FileLister lister (m_pFilterManager);

    m_toWatch.clear();
    m_toIndex.clear();

    lister.setFileCallbackFunction(&indexFileCallback);
    lister.setDirCallbackFunction(&watchDirCallback);

    // walk through user selected dirs
    for (set<string>::const_iterator iter = dirs.begin(); iter != dirs.end(); iter++)
        lister.listFiles(iter->c_str());

    // de-index files deleted since last strigi run
    map<string,time_t>::iterator mi = indexedFiles.begin();
    while (mi != indexedFiles.end())
    {
        map<string,time_t>::iterator it = m_toIndex.find(mi->first);

        if (it == m_toIndex.end())
        {
            // file has been deleted since last run
            events.push_back (new Event (Event::DELETED, mi->first));

            // no more useful, speedup into dirsRemoved
            map<string,time_t>::iterator itrm = mi;
            mi++;
            indexedFiles.erase(itrm);
        }
        else if (mi->second < it->second)
        {
            // file has been updated since last run
            events.push_back (new Event (Event::UPDATED, mi->first));
            m_toIndex.erase (it);
            mi++;
        }
        else
        {
            // file has NOT been changed since last run, we keep our indexed information
            m_toIndex.erase (it);
            mi++;
        }
    }

    // now m_toIndex contains only files created since the last run
    for (mi = m_toIndex.begin(); mi != m_toIndex.end(); mi++)
        events.push_back (new Event (Event::CREATED, mi->first));

    // add new watches
    addWatches (m_toWatch);

    m_toIndex.clear();
    m_toWatch.clear();

    if (events.size() > 0)
        m_pEventQueue->addEvents (events);
}

bool InotifyListener::indexFileCallback(const char* path, uint dirlen, uint len, time_t mtime)
{
    if (strstr(path, "/.")) return true;

    string file (path,len);

    (workingInotifyListener->m_toIndex).insert (make_pair(file, mtime));

    return true;
}

void InotifyListener::watchDirCallback(const char* path, uint len)
{
    string dir (path, len);

    (workingInotifyListener->m_toWatch).insert (dir);
}

string InotifyListener::eventToString(int events)
{
    string message;

    if ( (IN_ACCESS & events) != 0 )
        message += "ACCESS";
    else if ( (IN_MODIFY & events) != 0 )
        message += "MODIFY";
    else if ( (IN_ATTRIB & events) != 0 )
        message += "ATTRIB";
    else if ( (IN_CLOSE_WRITE & events) != 0 )
        message += "CLOSE_WRITE";
    else if ( (IN_CLOSE_NOWRITE & events) != 0 )
        message += "CLOSE_NOWRITE";
    else if ( (IN_OPEN & events) != 0 )
        message += "OPEN";
    else if ( (IN_MOVED_FROM & events) != 0 )
        message += "MOVED_FROM";
    else if ( (IN_MOVED_TO & events) != 0 )
        message += "MOVED_TO";
    else if ( (IN_CREATE & events) != 0 )
        message += "CREATE";
    else if ( (IN_DELETE & events) != 0 )
        message += "DELETE";
    else if ( (IN_DELETE_SELF & events) != 0 )
        message += "DELETE_SELF";
    else if ( (IN_UNMOUNT & events) != 0 )
        message += "UNMOUNT";
    else if ( (IN_Q_OVERFLOW & events) != 0 )
        message += " Q_OVERFLOW";
    else if ( (IN_IGNORED & events) != 0 )
        message += " IGNORED";
    else if ( (IN_CLOSE & events) != 0 )
        message += "CLOSE";
    else if ( (IN_MOVE & events) != 0 )
        message += "MOVE";
    else if ( (IN_ISDIR & events) != 0 )
        message += " ISDIR";
    else if ( (IN_ONESHOT & events) != 0 )
        message += " ONESHOT";
    else
        message = "UNKNOWN";

    return message;
}

//////////////////////////////////////
// ReindexDirsThread class implementation //
//////////////////////////////////////

InotifyListener::ReindexDirsThread::~ReindexDirsThread()
{
    for (unsigned int i = 0; i < m_events.size(); i++)
        delete m_events[i];

    m_events.clear();

    STRIGI_MUTEX_DESTROY(&m_nextJobLock);
    STRIGI_MUTEX_DESTROY(&m_resourcesLock);
}

void* InotifyListener::ReindexDirsThread::run(void*)
{
    workingReindex = this;

    while (getState() != Stopping)
    {
        sleep (1);
        if (!m_bfinished) // job isn't yet started, begin reindexing
            reindex();
        else
        {
            // reindexing already took place
            // maybe we've to start another job
            if ((STRIGI_MUTEX_TRY_LOCK (&m_resourcesLock)) && (STRIGI_MUTEX_TRY_LOCK (&m_nextJobLock)))
            {
                if ((!m_nextJobDirs.empty()) && (m_events.empty()))
                {
                    // m_nextJobDirs contains the new dirs to index
                    // m_events is empty bacause InotifyListener took all events related to previous job

                    m_oldDirs = m_newDirs;
                    m_newDirs = m_nextJobDirs;

                    // clear old structures
                    m_nextJobDirs.clear();
                    m_toWatch.clear();
                    m_toIndex.clear();
                    m_nomoreIndexedDirs.clear();
                    m_bfinished = false;
                }
                STRIGI_MUTEX_UNLOCK (&m_resourcesLock);
                STRIGI_MUTEX_UNLOCK (&m_nextJobLock);
            }
        }

        if (getState() == Working)
            setState(Idling);
    }

    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.run", string("exit state: ") + getStringState());
    return 0;
}

void InotifyListener::ReindexDirsThread::reindex () {
    if (!m_pIndexReader) {
        STRIGI_LOG_ERROR ("strigi.ReindexDirsThread.reindex", "m_pIndexReader == NULL!")
        return;
    }

    set<string> newIndexedDirs;
    string strNewIndexedDirs ("|");
    string strNomoreIndexedDirs ("|");

    // search for new dirs to index
    for (set<string>::iterator iter = m_newDirs.begin(); iter != m_newDirs.end(); iter++)
    {
        set<string>::iterator it = m_oldDirs.find(*iter);
        if (it == m_oldDirs.end())
        {
            newIndexedDirs.insert (*iter);
            strNewIndexedDirs += (*iter + "|");
        }
    }

    // search for dirs no more indexed
    for (set<string>::iterator iter = m_oldDirs.begin(); iter != m_oldDirs.end(); iter++)
    {
        set<string>::iterator it = m_newDirs.find(*iter);
        if (it == m_newDirs.end())
        {
            m_nomoreIndexedDirs.insert (*iter);
            strNomoreIndexedDirs += (*iter + "|");
        }
    }

    char buff [20];
    snprintf(buff, 20 * sizeof (char), "%i",m_nomoreIndexedDirs.size());
    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.reindex", string(buff) + " dirs are no more indexed");

    if (!m_nomoreIndexedDirs.empty())
        STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.reindex", "no more indexed dirs: " + strNomoreIndexedDirs);

    snprintf(buff, 20 * sizeof (char), "%i",newIndexedDirs.size());
    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.reindex", string(buff) + " new dirs are to indexed");

    if (!newIndexedDirs.empty())
        STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.reindex", "new indexed dirs: " + strNewIndexedDirs);

    FileLister lister (m_pFilterManager);

    m_toWatch.clear();
    m_toIndex.clear();

    lister.setFileCallbackFunction(&indexFileCallback);
    lister.setDirCallbackFunction(&watchDirCallback);

    // walk over the newly added dirs
    for (set<string>::const_iterator iter = newIndexedDirs.begin(); iter != newIndexedDirs.end(); iter++)
        lister.listFiles(iter->c_str());

    for (map<string,time_t>::iterator iter = m_toIndex.begin(); iter != m_toIndex.end(); iter++)
    {
        Event* event = new Event (Event::CREATED, iter->first);
        m_events.push_back (event);
    }

    m_bfinished = true;
}

void InotifyListener::ReindexDirsThread::setIndexedDirs (const set<string>& dirs)
{
    STRIGI_MUTEX_LOCK (&m_nextJobLock);
    m_nextJobDirs = dirs;
    STRIGI_MUTEX_UNLOCK (&m_nextJobLock);
}

bool InotifyListener::ReindexDirsThread::indexFileCallback(const char* path, uint dirlen, uint len, time_t mtime)
{
    if (strstr(path, "/.")) return true;

    string file (path,len);

    (workingReindex->m_toIndex).insert (make_pair(file, mtime));

    return true;
}

void InotifyListener::ReindexDirsThread::watchDirCallback(const char* path, uint len)
{
    string dir (path, len);

    (workingReindex->m_toWatch).insert (dir);
}

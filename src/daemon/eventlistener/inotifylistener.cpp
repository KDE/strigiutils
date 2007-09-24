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

#include "combinedindexmanager.h"
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

using namespace std;
using namespace Strigi;

class MatchString {
    string m_fixed_val;

    public:
        MatchString (string fixed_val) {m_fixed_val = fixed_val;}
        bool operator() (pair<int,string> val) {
            return (m_fixed_val.compare(val.second) == 0 ? true : false);
        }
};

////////////////////////////////////////////
// ReindexDirsThread class implementation //
///////////////////////////////////////////

/*!
 * @class ReindexDirsThread
 * @brief Simple thread called when user changes indexed dirs
 *
 * It's a separate thread that takes care of updating inotify watches and indexed files according to the new directories specified by the user
*/
class InotifyListener::ReindexDirsThread : public StrigiThread
{
    //friend class InotifyListener;

    public:
        explicit ReindexDirsThread (const char* name,
                                    const std::set<std::string> &olddirs);

        explicit ReindexDirsThread (const char* name);

        ~ReindexDirsThread();

        void* run(void*);

        void setIndexerConfiguration(Strigi::AnalyzerConfiguration* ic) {
            m_pindexerconfiguration = ic;
        }

        void setIndexedDirs (const std::set<std::string>& dirs,
                            const std::map<int, std::string>& watchedDirs);

        void setCombinedIndexManager (CombinedIndexManager* m) {
            m_pManager = m;
        }

        void getData(std::set<std::string>& noMoreIndexed,
                    std::set<std::string>& toWatch,
                    std::set<std::string>& newDirs,
                    std::vector<Event*>& events);

        bool working();

    protected:
        void cleanup();
        void reindex();

        void interrupt ();
        bool testInterrupt();
        void setWorking (bool value);

        CombinedIndexManager* m_pManager;
        Strigi::AnalyzerConfiguration* m_pindexerconfiguration;
        std::map<std::string, time_t> m_toIndex; //!< new files to index
        std::set<std::string> m_toWatch; //!< new directories to watch
        std::vector<Event*> m_events;
        std::set<std::string> m_newDirs; //!< new indexed dirs specified by the user
        std::set<std::string> m_oldDirs; //!< old indexed dirs
        std::set<std::string> m_nextJobDirs; //!< new dirs to index, user changed indexed dirs more than one time
        std::map<int, std::string> m_nextJobWatchedDirs;
        std::set<std::string> m_nomoreIndexedDirs; //!< dirs no more indexed
        pthread_mutex_t m_nextJobLock;//!< mutex lock over m_nextJobDirs
        pthread_mutex_t m_resourcesLock; //!< mutex lock over all variables (excluding m_nextJobDirs)
        pthread_mutex_t m_interruptLock;
        pthread_mutex_t m_workingLock;
        bool m_bWorking;
        bool m_bInterrupt;
        bool m_bInterrupted;
        bool m_bDataTaken;
        bool m_bHasWorkTodo;
};

InotifyListener::ReindexDirsThread::ReindexDirsThread (const char* name,
                                        const std::set<std::string> &olddirs)
: StrigiThread (name) {
    m_pManager = NULL;
    m_pindexerconfiguration = NULL;
    m_oldDirs = olddirs;
    m_bInterrupt = false;
    m_bInterrupted = false;
    m_bDataTaken = true;
    m_bHasWorkTodo = false;
    m_bWorking = false;
    STRIGI_MUTEX_INIT (&m_nextJobLock);
    STRIGI_MUTEX_INIT (&m_resourcesLock);
    STRIGI_MUTEX_INIT (&m_interruptLock);
    STRIGI_MUTEX_INIT (&m_workingLock);
}

InotifyListener::ReindexDirsThread::ReindexDirsThread (const char* name)
: StrigiThread (name) {
    m_pManager = NULL;
    m_pindexerconfiguration = NULL;
    m_newDirs.clear();
    m_oldDirs.clear();
    m_bInterrupt = false;
    m_bInterrupted = false;
    m_bDataTaken = true;
    m_bHasWorkTodo = false;
    m_bWorking = false;
    STRIGI_MUTEX_INIT (&m_nextJobLock);
    STRIGI_MUTEX_INIT (&m_resourcesLock);
    STRIGI_MUTEX_INIT (&m_interruptLock);
    STRIGI_MUTEX_INIT (&m_workingLock);
}

void InotifyListener::ReindexDirsThread::cleanup() {
    STRIGI_LOG_DEBUG( "strigi.ReindexDirsThread.cleanup",
                      "reindexing cancelled");

    //clean-up everything
    m_newDirs.clear();
    m_toWatch.clear();
    m_toIndex.clear();
    
    // free some memory
    for (vector<Event*>::iterator iter = m_events.begin();
         iter != m_events.end(); iter++)
    {
        if (*iter != NULL) {
            delete *iter;
            *iter = NULL;
        }
    }
    m_events.clear();
    
    m_bDataTaken = true;
    m_bInterrupt = false;
    m_bInterrupted = true;
    setWorking (false);
    
    STRIGI_MUTEX_UNLOCK (&m_resourcesLock);
}

InotifyListener::ReindexDirsThread::~ReindexDirsThread()
{
    for (unsigned int i = 0; i < m_events.size(); i++)
        delete m_events[i];

    m_events.clear();

    STRIGI_MUTEX_DESTROY(&m_nextJobLock);
    STRIGI_MUTEX_DESTROY(&m_resourcesLock);
    STRIGI_MUTEX_DESTROY(&m_interruptLock);
    STRIGI_MUTEX_DESTROY(&m_workingLock);
}

void InotifyListener::ReindexDirsThread::interrupt () {
    STRIGI_MUTEX_LOCK(&m_interruptLock);
    m_bInterrupt = true;
    STRIGI_MUTEX_UNLOCK(&m_interruptLock);
}

bool InotifyListener::ReindexDirsThread::testInterrupt() {
    bool value;
    STRIGI_MUTEX_LOCK(&m_interruptLock);
    value = m_bInterrupt;
    STRIGI_MUTEX_UNLOCK(&m_interruptLock);
    
    if (value)
        STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.testInterrupt",
                          "Caught interrupt event");
    
    return value;
}

void InotifyListener::ReindexDirsThread::getData(set<string>& noMoreIndexed,
        set<string>& toWatch,
        set<string>& newDirs,
        vector<Event*>& events)
{
    // assure all arrays are empty
    noMoreIndexed.clear();
    toWatch.clear();
    newDirs.clear();
    events.clear();
    
    if (STRIGI_MUTEX_TRY_LOCK (&m_resourcesLock) != 0)
        return;
    
    if (m_bDataTaken) {
        STRIGI_MUTEX_UNLOCK (&m_resourcesLock);
        return;
    }
    
    noMoreIndexed = m_nomoreIndexedDirs;
    toWatch = m_toWatch;
    events = m_events;
    newDirs = m_newDirs;
    
    m_toWatch.clear();
    m_nomoreIndexedDirs.clear();
    m_events.clear();
    
    m_bDataTaken = true;
    
    STRIGI_MUTEX_UNLOCK (&m_resourcesLock);
    
    char buffer [50];
    snprintf (buffer, 50 * sizeof (char), "%i", noMoreIndexed.size());
    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.getData",
                      string("dirs no more indexed: ") + buffer);
    snprintf (buffer, 50 * sizeof (char), "%i", toWatch.size());
    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.getData",
                      string("dirs to watch: ") + buffer);
    snprintf (buffer, 50 * sizeof (char), "%i", newDirs.size());
    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.getData",
                      string("dirs selected by the user: ") + buffer);
    snprintf (buffer, 50 * sizeof (char), "%i", events.size());
    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.getData",
                      string("events to process: ") + buffer);
}

void* InotifyListener::ReindexDirsThread::run(void*)
{
    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.run","starting");
    
    while (getState() != Stopping)
    {
        //TODO: increase value
        sleep (1);
        
        if (STRIGI_MUTEX_TRY_LOCK (&m_nextJobLock) == 0) {
            if ((m_bHasWorkTodo)
                    && STRIGI_MUTEX_TRY_LOCK (&m_resourcesLock) == 0) {
                // there's work to do, we've acquired locks over m_nextJobLock
                // and m_resourcesLock
            
                if (!m_bDataTaken) {
                    // previous reindex() run data has to be taken
                    STRIGI_MUTEX_UNLOCK (&m_nextJobLock);
                    STRIGI_MUTEX_UNLOCK (&m_resourcesLock);
                    continue;
                }

                setWorking (true);

                if (!m_bInterrupted)
                    m_oldDirs = m_newDirs;
                
                m_newDirs = m_nextJobDirs;
                m_nextJobDirs.clear();
                m_bHasWorkTodo = false;
                STRIGI_MUTEX_UNLOCK (&m_nextJobLock);
                
                // clear old structures
                m_toWatch.clear();
                m_toIndex.clear();
                m_events.clear();
                m_nomoreIndexedDirs.clear();
                
                if (m_newDirs == m_oldDirs) {
                    STRIGI_MUTEX_UNLOCK (&m_resourcesLock);
                    setWorking (false);
                    continue;
                }
                
                m_bDataTaken = false;
                
                reindex();
                
                setWorking (false);

                STRIGI_MUTEX_UNLOCK (&m_resourcesLock);
            }
            else
                STRIGI_MUTEX_UNLOCK (&m_nextJobLock);
        }
        
        if (getState() == Working)
            setState(Idling);
    }

    STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.run",
                      string("exit state: ") + getStringState());
    return 0;
}

void InotifyListener::ReindexDirsThread::reindex () {
    if (!m_pManager) {
        STRIGI_LOG_ERROR ("strigi.ReindexDirsThread.reindex",
                          "m_pManager == NULL!");
        return;
    }
    
    m_bInterrupted = false;
    
    map<int, string> watchedDirs = m_nextJobWatchedDirs;
    
    if (testInterrupt()) {
        cleanup();
        return;
    }
    
    m_toWatch.clear();
    m_toIndex.clear();
    
    Strigi::FileLister lister (m_pindexerconfiguration);
    
    for (set<string>::iterator iter = m_newDirs.begin();
         iter != m_newDirs.end(); iter++)
    {
        string filename;
        time_t mTime;
        
        lister.startListing( *iter);
        
        while (lister.nextFile( filename, mTime) != -1)
            m_toIndex.insert (make_pair (filename, mTime));

        //TODO: look for a better solution
        set<string> temp = lister.getListedDirs();
        for (set<string>::iterator it = temp.begin(); it != temp.end(); it++)
            m_toWatch.insert(*it);
    }
    
    map <string, time_t> indexedFiles = m_pManager->indexReader()->files(0);
    map<string,time_t>::iterator mi = indexedFiles.begin();

    while (mi != indexedFiles.end()) {
        map<string,time_t>::iterator it = m_toIndex.find(mi->first);

        if (it == m_toIndex.end()) {
            // file has been deleted since last run
            m_events.push_back (new Event (Event::DELETED, mi->first));

            // no more useful, speedup into dirsRemoved
            map<string,time_t>::iterator itrm = mi;
            mi++;
            indexedFiles.erase(itrm);
        }
        else if (mi->second < it->second) {
            // file has been updated since last run
            m_events.push_back (new Event (Event::UPDATED, mi->first));
            m_toIndex.erase (it);
            mi++;
        }
        else {
            // file has NOT been changed since last run,
            // we keep our indexed information
            m_toIndex.erase (it);
            mi++;
        }
    }

    // now m_toIndex contains only files created since the last run
    for (mi = m_toIndex.begin(); mi != m_toIndex.end(); mi++)
        m_events.push_back (new Event (Event::CREATED, mi->first));

    m_nomoreIndexedDirs.clear();
    for (map<int, string>::iterator it = watchedDirs.begin();
         it != watchedDirs.end(); it++)
    {
        set<string>::iterator match = m_toWatch.find(it->second);
        if (match == m_toWatch.end()) // dir is no longer watched
            m_nomoreIndexedDirs.insert(it->second);
        else // dir is already watched
            m_toWatch.erase (match);
    }
    
    if (testInterrupt()) {
        cleanup();
        return;
    }
}

bool InotifyListener::ReindexDirsThread::working()
{
    bool value;
    
    STRIGI_MUTEX_LOCK (&m_workingLock);
    value = m_bWorking;
    STRIGI_MUTEX_UNLOCK (&m_workingLock);
    
    return value;
}

void InotifyListener::ReindexDirsThread::setWorking(bool value)
{
    STRIGI_MUTEX_LOCK (&m_workingLock);
    m_bWorking = value;
    STRIGI_MUTEX_UNLOCK (&m_workingLock);
}

void InotifyListener::ReindexDirsThread::setIndexedDirs (
        const set<string>& dirs,
        const map<int,string>& watchedDirs)
{
    if (working()) {
        STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.setIndexedDirectories",
                          "going to interrupt previous reindexing operation");
        interrupt();
    }
    
    STRIGI_MUTEX_LOCK (&m_nextJobLock);
    m_nextJobDirs = dirs;
    m_nextJobWatchedDirs = watchedDirs;
    m_bHasWorkTodo = true;
    STRIGI_MUTEX_UNLOCK (&m_nextJobLock);
}

/* ----------- End ReindexDirsThread -------------------- */

InotifyListener::InotifyListener(set<string>& indexedDirs)
    :EventListener("InotifyListener")
{
    // listen only to interesting events
    m_iEvents = IN_CLOSE_WRITE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO
        | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF;

    m_bMonitor = true;
    setState(Idling);
    m_bInitialized = false;
    m_pollingListener = NULL;
    m_pReindexDirThread = NULL;
    STRIGI_MUTEX_INIT (&m_watchesLock);

    // fix path, all dir must end with a '/'
    for (set<string>::iterator iter = indexedDirs.begin();
         iter != indexedDirs.end(); iter++)
        m_indexedDirs.insert (fixPath(*iter));
}

InotifyListener::~InotifyListener()
{
    clearWatches();

    if (m_pollingListener != NULL) {
        m_pollingListener->stop();
        delete m_pollingListener;
        m_pollingListener = NULL;
    }

    if (m_pReindexDirThread != NULL) {
        m_pReindexDirThread->stop();
        delete m_pReindexDirThread;
        m_pReindexDirThread = NULL;
    }

    STRIGI_MUTEX_DESTROY (&m_watchesLock);
}

bool InotifyListener::init()
{
    m_iInotifyFD = inotify_init();
    if (m_iInotifyFD < 0) {
        STRIGI_LOG_ERROR ("strigi.InotifyListener",
                        "inotify_init() failed.  Are you running Linux 2.6.13\
                        or later? If so, something mysterious has gone wrong.");
        return false;
    }

    m_bInitialized = true;

    STRIGI_LOG_DEBUG ("strigi.InotifyListener", "successfully initialized");

    return true;
}

void* InotifyListener::run(void*)
{
    if (m_pReindexDirThread == NULL) {
        m_pReindexDirThread = new ReindexDirsThread ("ReindexDirThread");
        m_pReindexDirThread->setIndexerConfiguration(m_pindexerconfiguration);
        m_pReindexDirThread->setCombinedIndexManager( m_pManager);

        m_pReindexDirThread->start();
        
        m_pReindexDirThread->setIndexedDirs (m_indexedDirs, m_watches);
        // clear m_indexedDirs, ReindexDirsThread will set it by few seconds
        m_indexedDirs.clear();
    }
    
    while (getState() != Stopping) {
        watch();

        processReindexDirThreadData();

        if (getState() == Working)
            setState(Idling);
    }

    STRIGI_LOG_DEBUG ("strigi.InotifyListener.run",
                      string("exit state: ") + getStringState());
    return 0;
}

void InotifyListener::processReindexDirThreadData()
{
    if (m_pReindexDirThread == NULL) {
        STRIGI_LOG_WARNING("strigi.InotifyListener.processReindexDirThreadData",
                           "m_pReindexDirThread == NULL");
        return;
    }
    
    set<string> noMoreIndexed, toWatch, newDirs;
    vector<Event*> events;
    char buffer [50];

    m_pReindexDirThread->getData(noMoreIndexed, toWatch, newDirs, events);
    
    if (events.size() > 0) {
        snprintf (buffer, 50 * sizeof (char), "%i", events.size());
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.processReindexDirThreadData",
                          string("events to process (without delete events: ")
                                  + buffer + ")");
    }
    
    if (toWatch.size() > 0) {
        snprintf (buffer, 50 * sizeof (char), "%i", toWatch.size());
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.processReindexDirThreadData",
                          string("dirs to watch: ") + buffer);
        addWatches (toWatch, true);
    }
    
    if (newDirs.size() > 0) {
        snprintf (buffer, 50 * sizeof (char), "%i", newDirs.size());
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.processReindexDirThreadData",
                          string("dirs selected by the user: ") + buffer);
    }
    
    if (noMoreIndexed.size() > 0) {
        snprintf (buffer, 50 * sizeof (char), "%i", noMoreIndexed.size());
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.processReindexDirThreadData",
                          string("dirs no more indexed: ") + buffer);
        // remove old indexed files from db (interrupt enabled)
        rmWatches (noMoreIndexed, true);
    }

    if (!testInterrupt() && (events.size() > 0))
        m_pEventQueue->addEvents (events);

    // restore interrupt state
    setInterrupt (false);
    
    // TODO: lock?
    if (noMoreIndexed.size() != 0 || toWatch.size() != 0)
        m_indexedDirs = newDirs;
}

void InotifyListener::watch ()
{
    if (m_pEventQueue == NULL) {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.watch",
                          "m_pEventQueue == NULL!");
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
    
    while ( bytes < sizeof(struct inotify_event) ) {
        FD_ZERO(&read_fds);
        FD_SET(m_iInotifyFD, &read_fds);

        int rc = select(m_iInotifyFD + 1, &read_fds, NULL, NULL, &read_timeout);

        if ( rc < 0 ) {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.watch",
                              "Select on inotify failed");
            return;
        }
        else if ( rc == 0 ) {
            //Inotify select timeout
            return;
        }

        int thisBytes = read(m_iInotifyFD, &event + bytes, sizeof(struct inotify_event)*MAX_EVENTS - bytes);

        if ( thisBytes < 0 ) {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.watch",
                              "Read from inotify failed");
            return;
        }

        if ( thisBytes == 0 ) {
            STRIGI_LOG_WARNING ("strigi.InotifyListener.watch",
                                "Inotify reported end-of-file.\
                                Possibly too many events occurred at once.");
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

        if (watchIter == m_watches.end()) {
            if ((this_event->wd && IN_IGNORED) == 0) {
                // event wasn't marked with IGNORE flag, print some message

                char buff [20];
                snprintf(buff, 20 * sizeof (char), "%i",this_event->wd);

                STRIGI_LOG_WARNING ("strigi.InotifyListener.watch",
                    string("returned an unknown watch descriptor: ") + buff);

                string eventtype = eventToString (this_event->mask);

                STRIGI_LOG_WARNING ("strigi.InotifyListener.watch",
                                    "missed event type: " + eventtype);
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

        if (isEventInteresting (this_event)) {
            STRIGI_LOG_DEBUG ("strigi.InotifyListener.watch",
                              watchName + " changed");
            STRIGI_LOG_DEBUG ("strigi.InotifyListener.watch",
                               string("caught inotify event: ") +
                               eventToString( this_event->mask));

            if ((this_event->len > 0))
                STRIGI_LOG_DEBUG ("strigi.InotifyListener.watch",
                                  "event regards " +
                                  string(this_event->name, this_event->len));

            string file (watchName + string(this_event->name));

            if ( ((IN_MODIFY & this_event->mask) != 0) ||
                 ((IN_CLOSE_WRITE & this_event->mask) != 0) )
            {
                Event* event = new Event (Event::UPDATED, file);
                events.push_back (event);
            }
            else if (((IN_DELETE & this_event->mask) != 0) ||
                     ((IN_MOVED_FROM & this_event->mask) != 0 ) ||
                     ((IN_DELETE_SELF & this_event->mask) != 0 ) ||
                     ((IN_MOVE_SELF & this_event->mask) != 0 ))
            {
                if ((IN_ISDIR & this_event->mask) != 0)
                    dirRemoved (file, events);
                else {
                    Event* event = new Event (Event::DELETED, file);
                    events.push_back (event);
                }
            }
            else if (( (IN_CREATE & this_event->mask) != 0) ||
                       ((IN_MOVED_TO & this_event->mask) != 0 ) )
            {
                if ( (IN_ISDIR & this_event->mask) != 0 ) {
                    // a new directory has been created or an already watched
                    // directory has been moved into a watched place
                    
                    m_toIndex.clear();
                    m_toWatch.clear();

                    FileLister lister(m_pindexerconfiguration);
                    
                    string filename;
                    time_t mTime;
                    while (lister.nextFile( filename, mTime) != -1)
                        m_toIndex.insert (make_pair (filename, mTime));

                    m_toWatch = lister.getListedDirs();

                    for (map<string,time_t>::iterator i = m_toIndex.begin();
                         i != m_toIndex.end(); i++)
                    {
                        Event* event = new Event (Event::CREATED, i->first);
                        events.push_back (event);
                    }

                    // add new watches
                    addWatches (m_toWatch);

                    m_toWatch.clear();
                    m_toIndex.clear();
                }
                else {
                    Event* event = new Event (Event::CREATED, file);
                    events.push_back (event);
                }
            }
            else
                STRIGI_LOG_DEBUG ("strigi.InotifyListener.watch",
                                  "inotify's unknown event");
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
        STRIGI_LOG_ERROR ("strigi.InotifyListener.watch", message);
    }

    if (events.size() > 0)
        m_pEventQueue->addEvents (events);

    fflush( NULL );
}

bool
InotifyListener::isEventInteresting (struct inotify_event * event)
{
    // ignore files starting with '.'
    if (((IN_ISDIR & event->mask) == 0) && (event->len > 0)
          && ((event->name)[0] == '.'))
        return false;

    //TODO: FIX with AnalyzerConfiguration
//     if (m_pFilterManager != NULL)
//     {
//         if ((event->len > 0) && m_pFilterManager->findMatch(event->name, event->len))
//             return false;
//     }
//     else
//         STRIGI_LOG_WARNING ("strigi.InotifyListener.isEventInteresting", "unable to use filters, m_pFilterManager == NULL!")

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
            STRIGI_LOG_ERROR ("strigi.InotifyListener.addWatch",
                              "Failed to watch, maximum watch number reached");
            STRIGI_LOG_ERROR ("strigi.InotifyListener.addWatch",
                              "You've to increase the value stored into\
                               /proc/sys/fs/inotify/max_user_watches");
        }
        else if ( wd == -1 )
        {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.addWatch",
                              "Failed to watch " + path + " because of: " +
                               strerror(-wd));
        }
        else
        {
            char buff [20];
            snprintf(buff, 20* sizeof (char), "%i", wd);

            STRIGI_LOG_ERROR ("strigi.InotifyListener.addWatch",
                              "Failed to watch " + path + ": returned wd was " +
                               buff + " (expected -1 or >0 )");
        }

        return false;
    }
    else
    {
        m_watches.insert(make_pair(wd, path));

        STRIGI_MUTEX_UNLOCK (&m_watchesLock);

        STRIGI_LOG_INFO ("strigi.InotifyListener.addWatch",
                         "added watch for " + path);

        return true;
    }
}

void InotifyListener::addWatches (const set<string> &watches,
                                  bool enableInterrupt)
{
    set<string>::iterator iter;
    set<string> toPool;
    set<string> watched;

    for (iter = watches.begin(); iter != watches.end(); iter++)
    {
        if (enableInterrupt && testInterrupt())
            break;
        
        if (!addWatch (*iter))
        {

            if (errno == ENOSPC)
                 // user can't add no more watches, it's useless to go on
                break;

            // adding watch failed for other reason, keep trying with others
            toPool.insert(*iter);
        }
        else
            watched.insert (*iter);
    }

    if (iter != watches.end())
    {
        // probably we reached the max_user_watches limit
        for ( ; iter != watches.end(); iter++)
            toPool.insert (*iter);
    }

    
    if (enableInterrupt && testInterrupt())
    {
        //TODO: check
        
        vector <Event*> events;
        // recover from interrupt
        for (set<string>::iterator iter = watched.begin();
             iter != watched.end(); iter++)
        {
            dirRemoved (*iter, events);
        }
        
        m_pEventQueue->addEvents(events);
        
        setInterrupt (false);
    }
    else if (!toPool.empty())
    {
        if (m_pollingListener == NULL)
        {
            m_pollingListener = new PollingListener();
            m_pollingListener->setEventListenerQueue( m_pEventQueue);
            m_pollingListener->setCombinedIndexManager( m_pManager);
            m_pollingListener->setIndexerConfiguration(m_pindexerconfiguration);
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
        STRIGI_LOG_ERROR ("strigi.InotifyListener.rmWatch",
                           string("Error removing watch ") + buff +
                           " associated to path: " + path);
        STRIGI_LOG_ERROR ("strigi.InotifyListener.rmWatch",
                           string("error: ") + strerror(errno));
    }
    else
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.rmWatch",
                           string("Removed watch ") + buff + 
                           " associated to path: " + path);
    
    STRIGI_MUTEX_LOCK (&m_watchesLock);
    map<int, string>::iterator match = m_watches.find(wd);
    
    if (match != m_watches.end() && (path.compare(match->second) == 0))
        m_watches.erase (match);
    else
        STRIGI_LOG_ERROR ("strigi.InotifyListener.rmWatch",
                       "unable to remove internal watch reference for " + path);
    
    STRIGI_MUTEX_UNLOCK (&m_watchesLock);
}

void InotifyListener::rmWatches(map<int, string>& watchesToRemove,
                                bool enableInterrupt)
{
    set<string> removedWatches;
    map<int,string> watches;
    
    STRIGI_MUTEX_LOCK (&m_watchesLock);
    watches = m_watches;
    STRIGI_MUTEX_UNLOCK (&m_watchesLock);
    
    for (map<int,string>::iterator it = watchesToRemove.begin();
         it != watchesToRemove.end(); it++)
    {
        if (enableInterrupt && testInterrupt())
            break;
        
        map<int,string>::iterator match = watches.find (it->first);
        if (match != watches.end())
        {
            rmWatch (it->first, it->second);
            removedWatches.insert (it->second);
        }
        else
            STRIGI_LOG_WARNING ("strigi.InotifyListener.rmWatches", 
                        "unable to remove watch associated to " + it->second);
    }
    
    if (enableInterrupt && testInterrupt())
    {
        // undo the delete operation
        for (set<string>::iterator it = removedWatches.begin();
             it != removedWatches.end(); it++)
        {
            addWatch (*it);
        }
    }
}

void InotifyListener::rmWatches(set<string>& watchesToRemove,
                                bool enableInterrupt)
{
    map<int, string> removedWatches;
    
    STRIGI_MUTEX_LOCK (&m_watchesLock);
    // find all pairs <watch-id, watch-name> that have to be removed
    for (set<string>::iterator it = watchesToRemove.begin();
         it != watchesToRemove.end(); it++)
    {
        MatchString finder (*it);
        map<int, string>::iterator match = find_if (m_watches.begin(), m_watches.end(),
                                         finder);
        if (match != m_watches.end())
            removedWatches.insert (make_pair (match->first, match->second));
        else
            STRIGI_LOG_WARNING ("strigi.InotifyListener.rmWatches",
                "unable to find the watch associated to " + *it);
    }
    STRIGI_MUTEX_UNLOCK (&m_watchesLock);

    if (!enableInterrupt || !testInterrupt())
        rmWatches (removedWatches, enableInterrupt);
}

void InotifyListener::clearWatches ()
{
    STRIGI_MUTEX_LOCK (&m_watchesLock);

    for (map<int, string>::iterator iter = m_watches.begin();
         iter != m_watches.end(); iter++) 
    {
        rmWatch (iter->first, iter->second);
    }

    m_watches.clear();

    STRIGI_MUTEX_UNLOCK (&m_watchesLock);
}

void InotifyListener::dirRemoved (string dir, vector<Event*>& events,
                                  bool enableInterrupt)
{
    dir = fixPath (dir);
    vector<Event*> newEvents;
    map<int, string> watches;
    map<int, string> watchesToRemove;

    STRIGI_LOG_DEBUG ("strigi.InotifyListener.dirRemoved", dir +
            " is no longer watched, removing all indexed files contained");

    // we've to de-index all files contained into the deleted/moved directory
    if (m_pManager)
    {
        // all indexed files
        map<string, time_t> indexedFiles = m_pManager->indexReader()->files(0);

        // remove all entries that were contained into the removed dir
        for (map<string, time_t>::iterator it = indexedFiles.begin();
             it != indexedFiles.end(); it++)
        {
            if (enableInterrupt && testInterrupt())
                break;
            
            string::size_type pos = (it->first).find (dir);
            if (pos == 0)
            {
                Event* event = new Event (Event::DELETED, it->first);
                newEvents.push_back (event);
            }
        }
    }
    else
        STRIGI_LOG_WARNING ("strigi.InotifyListener.dirRemoved",
                            "m_pManager == NULL!");

    // remove inotify watches over no more indexed dirs
    STRIGI_MUTEX_LOCK (&m_watchesLock);
    watches = m_watches;
    STRIGI_MUTEX_UNLOCK (&m_watchesLock);
    
    for (map<int, string>::iterator mi = watches.begin();
         mi != watches.end(); mi++)
    {
        if ((mi->second).find (dir,0) == 0)
            watchesToRemove.insert (make_pair (mi->first, mi->second));
        
        if (enableInterrupt && testInterrupt())
            break;
    }
    
    if (enableInterrupt && testInterrupt())
    {
        for (vector<Event*>::iterator iter = newEvents.begin();
             iter != newEvents.end(); iter++)
        {
            delete *iter;
        }
        newEvents.clear();
    }
    else
    {
        rmWatches (watchesToRemove);
        
        for (vector<Event*>::iterator iter = newEvents.begin();
             iter != newEvents.end(); iter++)
        {
            events.push_back(*iter);
        }
        newEvents.clear();
    }
    
    // remove also dir watched by pollinglistener
    //FIXME: to fix, call right method
    if (m_pollingListener != NULL)
        m_pollingListener->rmWatch( dir);
}

void InotifyListener::dirsRemoved (set<string> dirs, vector<Event*>& events,
                                   bool enableInterrupt)
{
    vector<Event*> newEvents;
    
    for (set<string>::iterator iter = dirs.begin();
         iter != dirs.end(); iter++)
    {
        if (enableInterrupt && testInterrupt())
            break;
        
        dirRemoved (*iter, newEvents, enableInterrupt);
    }
    
    if (enableInterrupt && testInterrupt())
    {
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.dirsRemoved",
                          "recovering from interrupt signal");
        
        for (vector<Event*>::iterator iter = newEvents.begin();
             iter != newEvents.end(); iter++)
        {
            delete *iter;
        }
        newEvents.clear();
    }
    else
    {
        printf ("newEvents.size() == %i\n",events.size());
        for (vector<Event*>::iterator iter = newEvents.begin();
             iter != newEvents.end(); iter++)
        {
            events.push_back(*iter);
        }
        newEvents.clear();
    }
}

void InotifyListener::setInterrupt (bool value)
{
    STRIGI_MUTEX_LOCK(&m_interruptLock);
    m_bInterrupt = value;
    STRIGI_MUTEX_UNLOCK(&m_interruptLock);
}

bool InotifyListener::testInterrupt()
{
    bool value;
    STRIGI_MUTEX_LOCK(&m_interruptLock);
    value = m_bInterrupt;
    STRIGI_MUTEX_UNLOCK(&m_interruptLock);
    
    if (value)
        STRIGI_LOG_DEBUG ("strigi.ReindexDirsThread.testInterrupt",
                          "Caught interrupt event");
    
    return value;
}

void InotifyListener::setIndexedDirectories (const set<string> &dirs) {
    if (!m_pManager) {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.setIndexedDirectories",
                          "m_pManager == NULL!");
        return;
    }
    if (m_pEventQueue == NULL)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.setIndexedDirectories",
                          "m_pEventQueue == NULL!");
        return;
    }

    set<string> fixedDirs;

    // fix path, all dir must end with a '/'
    for (set<string>::iterator iter = dirs.begin(); iter != dirs.end(); iter++)
        fixedDirs.insert (fixPath (*iter));

    if (m_pReindexDirThread == NULL)
    {
        m_pReindexDirThread = new ReindexDirsThread ("ReindexDirThread",
                                                     m_indexedDirs);
        m_pReindexDirThread->setIndexerConfiguration(m_pindexerconfiguration);
        m_pReindexDirThread->setCombinedIndexManager ( m_pManager);

        m_pReindexDirThread->start();
    }
    
    if (m_pReindexDirThread->working())
        setInterrupt (true);
    
    STRIGI_MUTEX_LOCK (&m_watchesLock);
    map <int, string> watches = m_watches;
    STRIGI_MUTEX_UNLOCK (&m_watchesLock);
    
    m_pReindexDirThread->setIndexedDirs( fixedDirs, watches);

    string newdirs;

    for (set<string>::iterator iter = fixedDirs.begin(); iter != fixedDirs.end(); iter++)
        newdirs += ('|' + *iter);

    STRIGI_LOG_DEBUG ("strigi.InotifyListener.setIndexedDirectories",
                      "new indexed dirs: " + newdirs);
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

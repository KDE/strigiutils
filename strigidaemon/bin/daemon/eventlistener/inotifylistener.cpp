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

#include <strigi/analyzerconfiguration.h>
#include "combinedindexmanager.h"
#include "event.h"
#include "eventlistenerqueue.h"
#include <strigi/filelister.h>
#include <strigi/indexreader.h>
#include "../strigilogging.h"

#include <cerrno>
#include <cstring>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/types.h>
#include <vector>
#include <algorithm>

#include <sys/inotify.h>

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

InotifyEvent::InotifyEvent (int watchID, const string& watchName,
                            struct inotify_event* event)
    : FsEvent (watchName, event->name),
      m_event(event),
      m_watchName (watchName),
      m_watchID (watchID)
{
    if ( ((IN_MODIFY & m_event->mask) != 0) ||
           ((IN_CLOSE_WRITE & m_event->mask) != 0) )
        m_type = UPDATE;
    else if (  ((IN_DELETE & m_event->mask) != 0) ||
               ((IN_MOVED_FROM & m_event->mask) != 0 ) ||
               ((IN_DELETE_SELF & m_event->mask) != 0 ) ||
               ((IN_MOVE_SELF & m_event->mask) != 0 ))
        m_type = DELETE;
    else if ( ((IN_CREATE & m_event->mask) != 0) ||
              ((IN_MOVED_TO & m_event->mask) != 0 ) )
        m_type = CREATE;
    else
        STRIGI_LOG_DEBUG ("strigi.InotifyEvent.InotifyEvent",
                          "inotify's unknown event");
}

char* InotifyEvent::name()
{
    return m_event->name;
}

const string InotifyEvent::description()
{
    string message;

    if ( (IN_ACCESS & m_event->mask) != 0 )
        message += "ACCESS";
    else if ( (IN_MODIFY & m_event->mask) != 0 )
        message += "MODIFY";
    else if ( (IN_ATTRIB & m_event->mask) != 0 )
        message += "ATTRIB";
    else if ( (IN_CLOSE_WRITE & m_event->mask) != 0 )
        message += "CLOSE_WRITE";
    else if ( (IN_CLOSE_NOWRITE & m_event->mask) != 0 )
        message += "CLOSE_NOWRITE";
    else if ( (IN_OPEN & m_event->mask) != 0 )
        message += "OPEN";
    else if ( (IN_MOVED_FROM & m_event->mask) != 0 )
        message += "MOVED_FROM";
    else if ( (IN_MOVED_TO & m_event->mask) != 0 )
        message += "MOVED_TO";
    else if ( (IN_CREATE & m_event->mask) != 0 )
        message += "CREATE";
    else if ( (IN_DELETE & m_event->mask) != 0 )
        message += "DELETE";
    else if ( (IN_DELETE_SELF & m_event->mask) != 0 )
        message += "DELETE_SELF";
    else if ( (IN_UNMOUNT & m_event->mask) != 0 )
        message += "UNMOUNT";
    else if ( (IN_Q_OVERFLOW & m_event->mask) != 0 )
        message += " Q_OVERFLOW";
    else if ( (IN_IGNORED & m_event->mask) != 0 )
        message += " IGNORED";
    else if ( (IN_CLOSE & m_event->mask) != 0 )
        message += "CLOSE";
    else if ( (IN_MOVE & m_event->mask) != 0 )
        message += "MOVE";
    else if ( (IN_ISDIR & m_event->mask) != 0 )
        message += " ISDIR";
    else if ( (IN_ONESHOT & m_event->mask) != 0 )
        message += " ONESHOT";
    else
        message = "UNKNOWN";

    message += " event regarding ";
    message += string(m_event->name, m_event->len);
    message += " ; associated watch description: " + m_watchName;
    
    return message;
}

bool InotifyEvent::regardsDir()
{
    return ((IN_ISDIR & m_event->mask) != 0);
}

class InotifyListener::Private
{
    public:
        Private();

        virtual ~Private();

        bool init();

        bool isEventInteresting (FsEvent * event);
        bool isEventValid(FsEvent* event);

        void stopMonitoring();

        // event methods
        void pendingEvent(vector<FsEvent*>& events, unsigned int& counter);

        // watches methods
        bool addWatch (const std::string& path);
        void addWatches (const std::set<std::string>& watches);
        void rmWatch(int wd, std::string path);
        void rmWatches(std::map<int, std::string>& watchesToRemove);
        void rmWatches(std::set<std::string>& watchesToRemove);
        void clearWatches();

        void dirRemoved (string dir);

    private:
        int m_iInotifyFD;
        int m_iEvents;
        std::map<int, std::string> m_watches; //!< map containing all inotify watches added by InotifyListener. Key is watch descriptor, value is dir path
};


InotifyListener::Private::Private()
{
    // listen only to interesting events
    m_iEvents = IN_CLOSE_WRITE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO
                | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF;
}

InotifyListener::Private::~Private()
{
}

bool InotifyListener::Private::init()
{
    m_iInotifyFD = inotify_init();
    if (m_iInotifyFD < 0)
        return false;

    return true;
}

void InotifyListener::Private::pendingEvent(vector<FsEvent*>& events,
                                            unsigned int& counter)
{
    counter = 0;
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
            STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.pendingEvent",
                              "Select on inotify failed");
            return;
        }
        else if ( rc == 0 ) {
            //Inotify select timeout
            return;
        }
        
        int thisBytes = read(m_iInotifyFD, &event + bytes,
                             sizeof(struct inotify_event)*MAX_EVENTS - bytes);

        if ( thisBytes < 0 ) {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.pendingEvent",
                              "Read from inotify failed");
            return;
        }

        if ( thisBytes == 0 ) {
            STRIGI_LOG_WARNING ("strigi.InotifyListener.Private.pendingEvent",
                                "Inotify reported end-of-file."
                                "Possibly too many events occurred at once.");
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
        string watchName = "";
        int watchID = -1;
        
        this_event = (struct inotify_event *)this_event_char;

        map <int, string>::iterator watchIter = m_watches.find (this_event->wd);

        if (watchIter != m_watches.end()) {
            watchName = watchIter->second;
            watchID   = watchIter->first;
        }

        events.push_back (new InotifyEvent ( watchID, watchName, this_event));
        counter++;
        
        // next event
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
        STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.pendingEvent", message);
    }

    fflush( NULL );
}

bool InotifyListener::Private::addWatch (const string& path)
{
    map<int, string>::iterator iter;
    for (iter = m_watches.begin(); iter != m_watches.end(); ++iter)
    {
        if ((iter->second).compare (path) == 0) // dir is already watched
            return true;
    }

    static int wd;
    wd = inotify_add_watch (m_iInotifyFD, path.c_str(), m_iEvents);

    if (wd < 0)
    {
        if ((wd == -1) && ( errno == ENOSPC))
        {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.addWatch",
                              "Failed to watch, maximum watch number reached");
            STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.addWatch",
                              "You have to increase the value stored into\
                               /proc/sys/fs/inotify/max_user_watches");
        }
        else if ( wd == -1 )
        {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.addWatch",
                              "Failed to watch " + path + " because of: " +
                                      strerror(-wd));
        }
        else
        {
            char buff [20];
            snprintf(buff, 20* sizeof (char), "%i", wd);

            STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.addWatch",
                              "Failed to watch " + path + ": returned wd was " +
                                      buff + " (expected -1 or >0 )");
        }

        return false;
    }
    else
    {
        m_watches.insert(make_pair(wd, path));

        STRIGI_LOG_INFO ("strigi.InotifyListener.Private.addWatch",
                         "added watch for " + path);

        return true;
    }
}

void InotifyListener::Private::rmWatch(int wd, string path)
{
    char buff [20];
    snprintf(buff, 20 * sizeof (char), "%i",wd);

    if (inotify_rm_watch (m_iInotifyFD, wd) == -1)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.rmWatch",
                          string("Error removing watch ") + buff +
                                  " associated to path: " + path);
        STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.rmWatch",
                          string("error: ") + strerror(errno));
    }
    else
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.Private.rmWatch",
                          string("Removed watch ") + buff +
                                  " associated to path: " + path);
    
    map<int, string>::iterator match = m_watches.find(wd);
    
    if (match != m_watches.end() && (path.compare(match->second) == 0))
        m_watches.erase (match);
    else
        STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.rmWatch",
                       "unable to remove internal watch reference for " + path);
}

void InotifyListener::Private::rmWatches(map<int, string>& watchesToRemove)
{
    for (map<int,string>::iterator it = watchesToRemove.begin();
         it != watchesToRemove.end(); ++it)
    {
        map<int,string>::iterator match = m_watches.find (it->first);
        if (match != m_watches.end())
            rmWatch (it->first, it->second);
        else
            STRIGI_LOG_WARNING ("strigi.InotifyListener.Private.rmWatches",
                          "unable to remove watch associated to " + it->second);
    }
}

void InotifyListener::Private::rmWatches(set<string>& watchesToRemove)
{
    map<int, string> removedWatches;
    
    // find all pairs <watch-id, watch-name> that have to be removed
    for (set<string>::iterator it = watchesToRemove.begin();
         it != watchesToRemove.end(); ++it)
    {
        MatchString finder (*it);
        map<int, string>::iterator match = find_if (m_watches.begin(),
                m_watches.end(), finder);
        if (match != m_watches.end())
            removedWatches.insert (make_pair (match->first, match->second));
        else
            STRIGI_LOG_WARNING ("strigi.InotifyListener.Private.rmWatches",
                                "unable to find the watch associated to " + *it);
    }
    
    rmWatches (removedWatches);
}

void InotifyListener::Private::clearWatches ()
{
    map<int, string>::iterator iter;
    for (iter = m_watches.begin(); iter != m_watches.end(); ++iter) {
        char buff [20];
        snprintf(buff, 20 * sizeof (char), "%i", iter->first);

        if (inotify_rm_watch (m_iInotifyFD, iter->first) == -1)
        {
            STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.clearWatches",
                              string("Error removing watch ") + buff +
                                      " associated to path: " + iter->second);
            STRIGI_LOG_ERROR ("strigi.InotifyListener.Private.clearWatches",
                              string("error: ") + strerror(errno));
        }
        else
            STRIGI_LOG_DEBUG ("strigi.InotifyListener.Private.clearWatches",
                              string("Removed watch ") + buff +
                                      " associated to path: " + iter->second);
    }

    m_watches.clear();
}

bool InotifyListener::Private::isEventValid(FsEvent* event)
{
    InotifyEvent* inotifyEvent = dynamic_cast<InotifyEvent*> (event);

    if (inotifyEvent == 0)
        return false;

    map<int, string>::iterator match = m_watches.find(inotifyEvent->watchID());

    return (match != m_watches.end());
}

void InotifyListener::Private::dirRemoved (string dir)
{
    map <int, string> watchesToRemove;
    
    // remove inotify watches over no more indexed directories
    for (map<int, string>::iterator mi = m_watches.begin();
         mi != m_watches.end(); ++mi)
    {
        if ((mi->second).find (dir,0) == 0)
            watchesToRemove.insert (make_pair (mi->first, mi->second));
    }
    
    rmWatches (watchesToRemove);
}

// END InotifyListener::Private


InotifyListener::InotifyListener(set<string>& indexedDirs)
    :FsListener("InotifyListener", indexedDirs)
{
    p = new Private();
}

InotifyListener::~InotifyListener()
{
    clearWatches();
    delete p;

}

void InotifyListener::clearWatches ()
{
    p->clearWatches();
}

bool InotifyListener::init()
{
    if (!p->init()) {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.init",
                        "inotify_init() failed.  Are you running Linux 2.6.13\
                        or later? If so, something mysterious has gone wrong.");
        return false;
    }

    m_bInitialized = true;

    STRIGI_LOG_DEBUG ("strigi.InotifyListener.init","successfully initialized");

    return true;
}

FsEvent* InotifyListener:: retrieveEvent()
{
    if (m_events.empty())
        return 0;

    vector<FsEvent*>::iterator iter = m_events.begin();
    
    FsEvent* event = *iter;
    m_events.erase(iter);
    
    STRIGI_LOG_DEBUG("strigi.InotifyListener.retrieveEvent",
                     "returning " + event->description())
    
    return event;
}

bool InotifyListener::pendingEvent()
{
    unsigned int counter = 0;
    p->pendingEvent(m_events, counter);
    
    if (counter > 0) {
        char buff [20];
        snprintf(buff, 20 * sizeof (char), "%i", counter);
        string message = "Caught ";
        message += buff;
        message += " inotify's pending event(s)";
        
        STRIGI_LOG_DEBUG ("strigi.InotifyListener.pendingEvent", message)
    }
    
    dumpEvents();
    
    return !m_events.empty();
}

bool InotifyListener::isEventInteresting (FsEvent* event)
{
    InotifyEvent* inotifyEvent = dynamic_cast<InotifyEvent*> (event);

    if (inotifyEvent == 0)
        return false;
    
    struct inotify_event* structInotify = inotifyEvent->event();

    if ((structInotify->wd && IN_IGNORED) == 0)
        return false;

    // ignore files starting with '.'
    if (((IN_ISDIR & structInotify->mask) == 0) && (structInotify->len > 0)
          && ((structInotify->name)[0] == '.'))
        return false;


    if (m_pAnalyzerConfiguration == NULL) {
        STRIGI_LOG_WARNING ("strigi.InotifyListener.isEventInteresting",
                            "AnalyzerConfiguration == NULL")
        return true;
    }

    string path = inotifyEvent->watchName();
    if (path[path.length() - 1] != '/')
        path += '/';
    path += inotifyEvent->name();
    
    if (inotifyEvent->regardsDir())
        return m_pAnalyzerConfiguration->indexDir( path.c_str(),
                                                   inotifyEvent->name());
    else
        return m_pAnalyzerConfiguration->indexFile( path.c_str(),
                                                    inotifyEvent->name());
}

bool InotifyListener::isEventValid(FsEvent* event)
{
    return p->isEventValid (event);
}

bool InotifyListener::addWatch (const string& path)
{
    return p->addWatch (path);
}

void InotifyListener::dirRemoved (string dir, vector<Event*>& events)
{
    map<int, string> watchesToRemove;
    
    STRIGI_LOG_DEBUG ("strigi.InotifyListener.dirRemoved", dir +
            " is no longer watched, removing all indexed files contained");
    
    // we've to de-index all files contained into the deleted/moved directory
    if (m_pManager)
    {
        // all indexed files contained into directory
        map<string, time_t> indexedFiles;
        m_pManager->indexReader()->getChildren(dir, indexedFiles);

        // remove all entries that were contained into the removed directory
        for (map<string, time_t>::iterator it = indexedFiles.begin();
             it != indexedFiles.end(); ++it)
        {
            Event* event = new Event (Event::DELETED, it->first);
            events.push_back (event);
        }
    }
    else
        STRIGI_LOG_WARNING ("strigi.InotifyListener.Private.dirRemoved",
                            "m_pManager == NULL!");
    p->dirRemoved (dir);
}

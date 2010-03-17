/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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
#include "famlistener.h"

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

using namespace std;
using namespace Strigi;

class MatchString {
    string m_fixed_val;

    public:
        MatchString (string fixed_val) {m_fixed_val = fixed_val;}
        bool operator() (pair<FAMRequest,string> val) {
            return (m_fixed_val.compare(val.second) == 0 ? true : false);
        }
};

bool operator< (const FAMRequest& f1, const FAMRequest& f2)
{
    return f1.reqnum < f2.reqnum;
}

FamEvent::FamEvent (const string& watchName, FAMEvent event)
    : FsEvent (watchName, event.filename),
      m_event(event),
      m_watchName (watchName)
{
    bool doStat = true;
    
    switch (event.code)
    {
        case FAMChanged:
            m_type = UPDATE;
            break;
        case FAMMoved:
        case FAMDeleted:
            m_type = DELETE;
            break;
        case FAMCreated:
            m_type = CREATE;
            break;
        default:
            STRIGI_LOG_DEBUG ("strigi.FamEvent.FamEvent",
                              "unuseful event");
            doStat = false;
            break;
    }

    if (!doStat)
        return;
    
    struct stat status;

    string path = m_watchName;
    if (path[path.length() - 1] != '/')
        path += '/';
    path += event.filename;

    if ( stat( path.c_str(), &status) == 0) {
        if (S_ISDIR(status.st_mode))
            m_regardsDir = true;
        else
            m_regardsDir = false;
    }
    else {
        string msg;
        msg = "unable to execute stat() on FAMEvent.filename because of: ";
        msg += strerror (errno);
        STRIGI_LOG_ERROR ("strigi.FamEvent", msg)
    }
}

char* FamEvent::name()
{
    return m_event.filename;
}

const string FamEvent::description()
{
    string message;

    switch (m_event.code)
    {
        case FAMChanged:
            message += "FAMChanged";
            break;
        case FAMMoved:
            message += "FAMMoved";
            break;
        case FAMDeleted:
            message += "FAMDeleted";
            break;
        case FAMCreated:
            message += "FAMCreated";
            break;
        case FAMExists:
            message += "FAMExists";
            break;
        case FAMEndExist:
            message += "FAMEndExist";
            break;
        case FAMAcknowledge:
            message += "FAMAcknowledge";
            break;
        case FAMStartExecuting:
            message += "FAMStartExecuting";
            break;
        case FAMStopExecuting:
            message += "FAMStopExecuting";
            break;
    }
    
    message += " event regarding ";
    message += (m_regardsDir) ? "dir " : "file ";
    message += m_event.filename;
    message += " ; associated watch description: " + m_watchName;
    
    return message;
}

bool FamEvent::regardsDir()
{
    return m_regardsDir;
}

class FamListener::Private
{
    public:
        Private();

        virtual ~Private();

        bool init();
        void stopMonitoring();

        bool isEventInteresting (FsEvent * event);
        bool isEventValid(FsEvent* event);

        // event methods
        void pendingEvent(vector<FsEvent*>& events, unsigned int& counter);

        // dir methods
        void dirRemoved (string dir);
        
        // watches methods
        bool addWatch (const std::string& path);
        void addWatches (const std::set<std::string>& watches);
        void rmWatch(FAMRequest& famRequest, std::string path);
        void rmWatches(std::map<FAMRequest, std::string>& watchesToRemove);
        void rmWatches(std::set<std::string>& watchesToRemove);
        void clearWatches();

    private:
        FAMConnection m_famConnection;
        std::map<FAMRequest, std::string> m_watches; //!< map containing all inotify watches added by FamListener. Key is watch descriptor, value is dir path
};

bool FamListener::Private::init()
{
    if (FAMOpen(&m_famConnection) == -1)
        return false;
    
    return true;
}

FamListener::Private::Private()
{
}

FamListener::Private::~Private()
{
    clearWatches();
    
    if (FAMClose (&m_famConnection) == -1)
        STRIGI_LOG_ERROR ("strigi.FamListener",
                          "Error during FAM close procedure");
}

void FamListener::Private::pendingEvent(vector<FsEvent*>& events,
                                        unsigned int& counter)
{
    sleep (1);
    
    if (FAMPending(&m_famConnection)) {
        FAMEvent event;
        if (FAMNextEvent (&m_famConnection, &event) == -1) {
            STRIGI_LOG_ERROR ("strigi.FamListener.pendingEvent",
                              "Fam event retrieval failed");
            return;
        }

        if ((event.code == FAMChanged) || (event.code == FAMMoved) ||
             (event.code == FAMDeleted) || (event.code == FAMCreated))
        {
            map<FAMRequest, string>::iterator match;
            match = m_watches.find (event.fr);
            if (match != m_watches.end()) {
                events.push_back (new FamEvent (match->second, event));
                counter++;
            }
        }
    }
}

bool FamListener::Private::isEventValid(FsEvent* event)
{
    FamEvent* famEvent = dynamic_cast<FamEvent*> (event);

    if (famEvent == 0)
        return false;

    map<FAMRequest, string>::iterator match = m_watches.find(famEvent->fr());

    return (match != m_watches.end());
}

bool FamListener::Private::addWatch (const string& path)
{
    map<FAMRequest, string>::iterator iter;
    for (iter = m_watches.begin(); iter != m_watches.end(); ++iter) {
        if ((iter->second).compare (path) == 0) // dir is already watched
            return true;
    }

    FAMRequest famRequest;
    if (FAMMonitorDirectory (&m_famConnection, path.c_str(), &famRequest, 0) == 0) {
        m_watches.insert(make_pair(famRequest, path));
        return true;
    }
    else
        return false;
}

void FamListener::Private::rmWatch(FAMRequest& famRequest, string path)
{
    if (FAMCancelMonitor (&m_famConnection, &famRequest) == -1)
        STRIGI_LOG_ERROR ("strigi.FamListener.Private.rmWatch",
                    string("Error removing watch associated to path: ") + path)
    else
        STRIGI_LOG_DEBUG ("strigi.FamListener.Private.rmWatch",
                    string("Removed watch associated to path: ") + path)
    
    map<FAMRequest, string>::iterator match = m_watches.find(famRequest);
    
    if (match != m_watches.end() && (path.compare(match->second) == 0))
        m_watches.erase (match);
    else
        STRIGI_LOG_ERROR ("strigi.FamListener.Private.rmWatch",
                          "unable to remove internal watch reference for " + path)
}

void FamListener::Private::rmWatches(map<FAMRequest, string>& watchesToRemove)
{
    for (map<FAMRequest,string>::iterator it = watchesToRemove.begin();
         it != watchesToRemove.end(); ++it)
    {
        map<FAMRequest,string>::iterator match = m_watches.find (it->first);
        if (match != m_watches.end()) {
            FAMRequest famRequest = it->first;
            rmWatch ( famRequest, it->second);
        }
        else
            STRIGI_LOG_WARNING ("strigi.FamListener.Private.rmWatches",
                        "unable to remove watch associated to " + it->second);
    }
}

void FamListener::Private::rmWatches(set<string>& watchesToRemove)
{
    map<FAMRequest, string> removedWatches;
    
    // find all pairs <watch-id, watch-name> that have to be removed
    for (set<string>::iterator it = watchesToRemove.begin();
         it != watchesToRemove.end(); ++it)
    {
        MatchString finder (*it);
        map<FAMRequest, string>::iterator match;
        match = find_if (m_watches.begin(), m_watches.end(), finder);
        
        if (match != m_watches.end())
            removedWatches.insert (make_pair (match->first, match->second));
        else
            STRIGI_LOG_WARNING ("strigi.FamListener.Private.rmWatches",
                                "unable to find the watch associated to " + *it)
    }
    
    rmWatches (removedWatches);
}

void FamListener::Private::clearWatches ()
{
    map<FAMRequest, string>::iterator iter;
    for (iter = m_watches.begin(); iter != m_watches.end(); ++iter) {
        FAMRequest famRequest = iter->first;
        if (FAMCancelMonitor (&m_famConnection, &famRequest) == -1)
            STRIGI_LOG_ERROR ("strigi.FamListener.rmWatch",
             string("Error removing watch associated to path: ") + iter->second)
        else
            STRIGI_LOG_DEBUG ("strigi.FamListener.rmWatch",
                   string("Removed watch associated to path: ") + iter->second)
    }

    m_watches.clear();
}

void FamListener::Private::dirRemoved (string dir)
{
    map<FAMRequest, string> watchesToRemove;

    // remove inotify watches over no more indexed dirs
    for (map<FAMRequest, string>::iterator mi = m_watches.begin();
         mi != m_watches.end(); ++mi)
    {
        if ((mi->second).find (dir,0) == 0)
            watchesToRemove.insert (make_pair (mi->first, mi->second));
    }
    
    rmWatches (watchesToRemove);
}

// END FamListener::Private

FamListener::FamListener(set<string>& indexedDirs)
    :FsListener("FamListener", indexedDirs)
{
    p = new Private();
}

FamListener::~FamListener()
{
    delete p;
}

bool FamListener::init()
{
    if (!p->init()) {
        STRIGI_LOG_ERROR ("strigi.FamListener.init",
                          "Error during FAM initialization");
        return false;
    }

    m_bInitialized = true;

    STRIGI_LOG_DEBUG ("strigi.FamListener.init", "successfully initialized");

    return true;
}

FsEvent* FamListener:: retrieveEvent()
{
    if (m_events.empty())
        return 0;

    vector<FsEvent*>::iterator iter = m_events.begin();
    
    FsEvent* event = *iter;
    m_events.erase(iter);
    return event;
}

bool FamListener::pendingEvent()
{
    unsigned int counter = 0;
    p->pendingEvent (m_events, counter);

    if (counter > 0) {
        char buff [20];
        snprintf(buff, 20 * sizeof (char), "%i", counter);
        string message = "Caught ";
        message += buff;
        message += " FAM event(s)";
    
        STRIGI_LOG_DEBUG ("strigi.FamListener.pendingEvent", message)
    }
    
    return !m_events.empty();
}

bool FamListener::isEventInteresting (FsEvent* event)
{
    FamEvent* famEvent = dynamic_cast<FamEvent*> (event);

    if (famEvent == 0)
        return false;
    
    // ignore directories starting with '.'
    if ((famEvent->regardsDir()) &&
        (strlen (famEvent->name()) > 0) && (famEvent->name())[0] == '.')
        return false;

    if (m_pAnalyzerConfiguration == NULL) {
        STRIGI_LOG_WARNING ("strigi.FamListener.isEventInteresting",
                            "AnalyzerConfiguration == NULL")
        return true;
    }

    string path = famEvent->watchName();
    if (path[path.length() - 1] != '/')
        path += '/';
    path += famEvent->name();
    
    if (famEvent->regardsDir())
        return m_pAnalyzerConfiguration->indexDir( path.c_str(),
                                                   famEvent->name());
    else
        return m_pAnalyzerConfiguration->indexFile( path.c_str(),
                                                    famEvent->name());
}

bool FamListener::isEventValid(FsEvent* event)
{
    return p->isEventValid ( event);
}

bool FamListener::addWatch (const string& path)
{
    if (p->addWatch (path)) {
        STRIGI_LOG_INFO ("strigi.FamListener.addWatch",
                         "added watch for " + path);
        return true;
    }
    else {
        STRIGI_LOG_ERROR ("strigi.FamListener.addWatch",
                          "Failed to watch " + path)
        return false;
    }
}

void FamListener::clearWatches ()
{
    p->clearWatches();
}

void FamListener::dirRemoved (string dir, vector<Event*>& events)
{
    map<FAMRequest, string> watchesToRemove;

    STRIGI_LOG_DEBUG ("strigi.FamListener.dirRemoved", dir +
            " is no longer watched, removing all indexed files contained")

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
        STRIGI_LOG_WARNING ("strigi.FamListener.dirRemoved",
                            "m_pManager == NULL!");

    p->dirRemoved (dir);
}

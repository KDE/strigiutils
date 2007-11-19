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
#include "fslistener.h"

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
#include <sstream>
#include <vector>

using namespace std;
using namespace Strigi;

namespace {
    /*!
    * @param path string containing path to check
    * Appends the terminating char to path.
    * Under Windows that char is '\', '/' under *nix
    */
    string fixPath (string path)
    {
        if ( path.c_str() == NULL || path.length() == 0 )
            return "";

        string temp(path);

    #ifdef HAVE_WINDOWS_H
        size_t l= temp.length();
        char* t = (char*)temp.c_str();
        for (size_t i=0;i<l;i++){
            if ( t[i] == '\\' )
                t[i] = '/';
        }
        temp[0] = tolower(temp.at(0));
    #endif

        char separator = '/';

        if (temp[temp.length() - 1 ] != separator)
            temp += separator;

        return temp;
    }
}

void calculateDiff(set<string> actualDirs, set<string> reindexDirs,
                   set<string>& dirsDeleted,set<string>& dirsCreated)
{
    set<string>::iterator iter;
    for (iter = actualDirs.begin(); iter != actualDirs.end(); iter++) {
        set<string>::iterator match = reindexDirs.find (*iter);
        if (match == reindexDirs.end())
            dirsDeleted.insert (*iter);
        else
            reindexDirs.erase (iter);
    }

    for (iter = reindexDirs.begin(); iter != reindexDirs.end(); iter++)
        dirsCreated.insert (*iter);
}

class File
{
    public:
        File(string name, time_t mtime)
            : m_name (name),
              m_mtime (mtime)
            {};

        string m_name;
        time_t m_mtime;
};

bool operator< (const File& f1, const File& f2)
{
    if (f1.m_name.compare(f2.m_name) < 0)
        return true;
    else
        return false;
}

class MatchFile
{
    string m_name;
    public:
        MatchFile( string name)
            : m_name (name) {};

        bool operator ()(const File& f)
            { return (m_name == f.m_name); }
};

// improved multimap, stl multimap class doesn't provide a method that returns
// all the available keys
typedef map< string, set<File> > iMultimap;


FsListener::FsListener(const char* name, set<string>& indexedDirs)
    : EventListener(name)
{
    m_bMonitor = true;
    setState(Idling);
    m_bInitialized = false;
    m_bBootstrapped = false;
    m_indexedDirs = indexedDirs;
    
    STRIGI_MUTEX_INIT (&m_reindexLock);
}

FsListener::~FsListener()
{
    clearWatches();

    STRIGI_MUTEX_DESTROY (&m_reindexLock);
}

void* FsListener::run(void*)
{
    while (getState() != Stopping) {
        if (!m_bBootstrapped)
            bootstrap();
        else if (reindexReq())
            reindex();
        else
            watch();

        if (getState() == Working)
            setState(Idling);
    }

    STRIGI_LOG_DEBUG ("strigi.FsListener.run",
                      string("exit state: ") + getStringState());
    return 0;
}

bool FsListener::reindexReq()
{
    bool ret;
    
    STRIGI_MUTEX_LOCK (&m_reindexLock);
    ret = m_bReindexReq;
    STRIGI_MUTEX_UNLOCK (&m_reindexLock);

    return ret;
}

void FsListener::getReindexDirs(set<string>& reindexDirs)
{
    STRIGI_MUTEX_LOCK (&m_reindexLock);
    m_bReindexReq = false;
    reindexDirs = m_reindexDirs;
    m_reindexDirs.clear();
    STRIGI_MUTEX_UNLOCK (&m_reindexLock);
}

void FsListener::bootstrap()
{
    STRIGI_LOG_DEBUG ("strigi.FsListener.bootstrap","BOOTSTRAP INIT");
    
    set<string> toWatch;
    vector<Event*> events;
    iMultimap files;
    set<string> bootstrapDirs;

    if (reindexReq())
        getReindexDirs ( bootstrapDirs);
    else
        bootstrapDirs = m_indexedDirs;
    
    for (set<string>::iterator iter = bootstrapDirs.begin();
         iter != bootstrapDirs.end(); iter++)
    {
        DirLister lister(m_pindexerconfiguration);
        string path;
        vector<pair<string, struct stat> > dirs;

        lister.startListing (*iter);
        int ret = lister.nextDir(path, dirs);

        while (ret != -1) {
            if (reindexReq())
                break;
            
            vector<pair<string, struct stat> >::iterator iter;
            
            for (iter = dirs.begin(); iter != dirs.end(); iter++) {
                struct stat stats = iter->second;

                if (S_ISDIR(stats.st_mode)) { //dir
                    toWatch.insert (iter->first);
                }
                else if (S_ISREG(stats.st_mode)) { //file
                    iMultimap::iterator mIter = files.find (path);
                    File file (iter->first, stats.st_mtime);
                    if (mIter != files.end())
                        (mIter->second).insert (file);
                    else {
                        set<File> temp;
                        temp.insert (file);
                        files.insert(make_pair(path, temp));
                    }
                }
            }
            
            ret = lister.nextDir(path, dirs);
        }
    }

    stringstream msg;
    msg << "there're " << files.size();
    msg << " keys inside iMultimap";
    STRIGI_LOG_DEBUG ("strigi.FsListener.bootstrap", msg.str())
    
    for (iMultimap::iterator iter = files.begin(); iter != files.end(); iter++)
    {
        map <string, time_t> indexedFiles;
        stringstream msg;
        string path = iter->first;

        // retrieve all indexed files contained by path
        m_pManager->indexReader()->getChildren (path, indexedFiles);
        msg << "there're " << indexedFiles.size();
        msg << " indexed files associated to dir " << path;

        STRIGI_LOG_DEBUG ("strigi.FsListener.bootstrap", msg.str())

        if (!indexedFiles.empty()) {
            // find differences between fs files and indexed ones
            map<string, time_t>::iterator it = indexedFiles.begin();
            for ( ; it != indexedFiles.end(); it++) {
                MatchFile finder (it->first);
                set<File>::iterator match;
                match = find_if( (iter->second).begin(),
                                 (iter->second).end(), finder);


                if (match == (iter->second).end()) {
                    // indexed file has been deleted from filesystem
                    events.push_back (new Event (Event::DELETED, iter->first));
                }
                else if ((*match).m_mtime > it->second) {
                    // file has been updated
                    events.push_back (new Event (Event::UPDATED, iter->first));
                }
                else {
                    // file has not been modified since index time
                    (iter->second).erase( match);
                }

                if (reindexReq())
                    break;
            }
        }

        for (set<File>::iterator it = (iter->second).begin();
             it != (iter->second).end(); it++)
        {
            File file = *it;
            events.push_back (new Event (Event::CREATED, file.m_name));
        }
    }

    //TODO: check
    if (reindexReq()) {
        for (vector<Event*>::iterator iter = events.begin();
             iter != events.end(); iter++) {
            delete *iter;
        }

        events.clear();
    }
    else {
        if (events.size() > 0)
            m_pEventQueue->addEvents (events);

        addWatches (toWatch);

        m_bBootstrapped = true;
        m_indexedDirs = bootstrapDirs;
    }
}

void FsListener::reindex()
{
    STRIGI_LOG_DEBUG ("strigi.FsListener.reindex","REINDEX INIT");
    
    if (m_pEventQueue == NULL) {
        STRIGI_LOG_ERROR ("strigi.FsListener.reindex",
                          "m_pEventQueue == NULL!");
        return;
    }
    
    set<string> reindexDirs;
    set<string> dirsDeleted;
    set<string> dirsCreated;
    set<string> dirsMonitored;
    vector<Event*> events;
    stringstream message;
    
    if (!reindexReq ())
        return;

    getReindexDirs(reindexDirs);
    
    calculateDiff(m_indexedDirs, reindexDirs, dirsDeleted, dirsCreated);

    message << dirsCreated.size() << " new dir(s); ";
    message << dirsDeleted.size() << " deleted dir(s)";
    STRIGI_LOG_DEBUG ("strigi.FsListener.reindex", message.str())

    for (set<string>::iterator iter = dirsCreated.begin();
         iter != dirsCreated.end() && !reindexReq(); iter++)
    {
        DirLister lister(m_pindexerconfiguration);
        string path;
        vector<pair<string, struct stat> > dirs;

        lister.startListing (*iter);
        int ret = lister.nextDir(path, dirs);

        while (ret != -1) {
            vector<pair<string, struct stat> >::iterator iter;
            for (iter = dirs.begin(); iter != dirs.end(); iter++) {
                struct stat stats = iter->second;
                if (S_ISDIR(stats.st_mode)) {
                    //dir
                    recursivelyMonitor (iter->first, events);
                    dirsMonitored.insert (iter->first);
                }
                else if (S_ISREG(stats.st_mode)) {
                    //file
                    events.push_back (new Event (Event::CREATED, iter->first));
                }
            }
            ret = lister.nextDir(path, dirs);
        }
    }

    if (reindexReq()) {
        // another reindex request arrived, undo last actions
        for (vector<Event*>::iterator iter = events.begin();
             iter != events.end(); iter++)
            delete *iter;
        events.clear();
        //TODO check!!!
        dirsRemoved (dirsMonitored, events);
    }
    else {
        // finish reindex operation
        dirsRemoved (dirsDeleted, events);

        if (events.size() > 0)
            m_pEventQueue->addEvents (events);
        
        //update indexedDirs
        m_indexedDirs = reindexDirs;
    }
}

void FsListener::watch ()
{
    if (m_pEventQueue == NULL) {
        STRIGI_LOG_ERROR ("strigi.FsListener.watch",
                          "m_pEventQueue == NULL!");
        return;
    }

    //some code taken from inotify-tools (http://inotify-tools.sourceforge.net/)

    vector <Event*> events;

    while (pendingEvent()) {
        FsEvent* fsevent  = retrieveEvent();

        if (!isEventValid(fsevent)) {
            STRIGI_LOG_WARNING ("strigi.FsListener.watch",
                                "discarded invalid event");
            continue;
        }

        if (isEventInteresting(fsevent)) {
            STRIGI_LOG_DEBUG ("strigi.FsListener.watch",
                              fsevent->description());

            switch (fsevent->type()) {
                case FsEvent::UPDATE:
                {
                    Event* event = new Event (Event::UPDATED, fsevent->file());
                    events.push_back (event);
                    break;
                }
                case FsEvent::DELETE:
                {
                    if (fsevent->regardsDir())
                        dirRemoved (fsevent->file(), events);
                    else {
                        Event* event = new Event (Event::DELETED,
                                                  fsevent->file());
                        events.push_back (event);
                    }
                    break;
                }
                case FsEvent::CREATE:
                {
                    if (fsevent->regardsDir())
                        recursivelyMonitor (fsevent->file(), events);
                    else {
                        Event* event = new Event (Event::CREATED,
                                                  fsevent->file());
                        events.push_back (event);
                    }
                    break;
                }
            }
        }
    }

    if (events.size() > 0)
        m_pEventQueue->addEvents (events);
}

void FsListener::recursivelyMonitor (string dir, vector<Event*>& events)
{
    STRIGI_LOG_DEBUG ("FsListener.recursivelyMonitor","going to monitor " + dir)
    DirLister lister(m_pindexerconfiguration);
    string path;
    vector<pair<string, struct stat> > fsitems;
    set<string> to_watch;

    lister.startListing (dir);
    int ret = lister.nextDir(path, fsitems);

    while (ret != -1) {
        for (vector<pair<string,struct stat> >::iterator iter = fsitems.begin();
             iter != fsitems.end(); iter++)
        {
            struct stat stats = iter->second;
            
            if (S_ISDIR(stats.st_mode)) //dir
                to_watch.insert (iter->first);
            else if (S_ISREG(stats.st_mode)) {
                //file
                Event* event = new Event (Event::CREATED, iter->first);
                events.push_back (event);
            }
        }
        ret = lister.nextDir(path, fsitems);
    }
    
    // add new watches
    to_watch.insert (dir);
    addWatches (to_watch);
}

void FsListener::dirsRemoved (set<string> dirs, vector<Event*>& events)
{
    for (set<string>::iterator iter = dirs.begin();
         iter != dirs.end(); iter++)
        dirRemoved (fixPath(*iter), events);
}

void FsListener::setIndexedDirectories (const set<string> &dirs)
{
    stringstream msg;
    set<string> fixedDirs;

    // fix path, all dir must end with a '/'
    for (set<string>::iterator iter = dirs.begin(); iter != dirs.end(); iter++)
        fixedDirs.insert (fixPath (*iter));

    STRIGI_MUTEX_LOCK (&m_reindexLock);
    m_reindexDirs = fixedDirs;
    m_bReindexReq = true;
    STRIGI_MUTEX_UNLOCK (&m_reindexLock);


    msg << fixedDirs.size() << " dirs specified";
    STRIGI_LOG_DEBUG ("FsListener.setIndexedDirectories", msg.str())
}

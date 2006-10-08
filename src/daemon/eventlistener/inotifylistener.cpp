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
#include "filtermanager.h"
#include "filelister.h"
#include "indexreader.h"
#include "../strigilogging.h"

#include <cerrno>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/types.h>
#include <vector>

#include "inotify.h"
#include "inotify-syscalls.h"

InotifyListener* iListener;

using namespace std;
using namespace jstreams;

InotifyListener::InotifyListener(set<string>& indexedDirs)
    :EventListener("InotifyListener")
{
    iListener = this;

    // listen only to interesting events
    m_iEvents = IN_CLOSE_WRITE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO
        | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF;

    m_bMonitor = true;
    setState(Idling);
    m_bInitialized = false;
    m_indexedDirs = indexedDirs;
    m_pollingListener = new PollingListener();
}

InotifyListener::~InotifyListener()
{
    clearWatches();
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

        if (getState() == Working)
            setState(Idling);
    }
    
    STRIGI_LOG_DEBUG ("strigi.InotifyListener.run", string("exit state: ") + getStringState());
    return 0;
}

void InotifyListener::watch ()
{
    if (m_eventQueue == NULL)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.watch", "m_eventQueue == NULL!")
        return;
    }

    // some code taken from inotify-tools (http://inotify-tools.sourceforge.net/)

    vector <Event*> events;
    set <unsigned int> watchesToDel;

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
            STRIGI_LOG_ERROR ("strigi.InotifyListener", "Select on inotify failed")
            return;
        }
        else if ( rc == 0 )
        {
             // cerr << "Inotify: Select timeout\n";
            return;
        }

        int thisBytes = read(m_iInotifyFD, &event + bytes, sizeof(struct inotify_event)*MAX_EVENTS - bytes);

        if ( thisBytes < 0 )
        {
            STRIGI_LOG_ERROR ("strigi.InotifyListener", "Read from inotify failed")
            return;
        }

        if ( thisBytes == 0 )
        {
            STRIGI_LOG_WARNING ("strigi.InotifyListener", "Inotify reported end-of-file.  Possibly too many events occurred at once.")
            return;
        }

        bytes += thisBytes;
    }

    map < unsigned int, string>::iterator iter;

    static struct inotify_event * this_event;

    static char * this_event_char;
    this_event_char = (char *)&event[0];

    static uint remaining_bytes;
    remaining_bytes = bytes;

    do
    {
        string message;

        this_event = (struct inotify_event *)this_event_char;

        iter = m_watches.find (this_event->wd);

        if (iter == m_watches.end())
        {
            char buff [20];
            snprintf(buff, 20 * sizeof (char), "%i",this_event->wd);
            
            STRIGI_LOG_WARNING ("strigi.InotifyListener", string("returned an unknown watch descriptor: ") + buff)
            
            // jump to next event
            this_event_char += sizeof(struct inotify_event) + this_event->len;
            remaining_bytes -= sizeof(struct inotify_event) + this_event->len;
            continue;
        }

        if (isEventInteresting (this_event))
        {
            STRIGI_LOG_DEBUG ("strigi.InotifyListener", iter->second + " changed")
                    STRIGI_LOG_DEBUG ("strigi.InotifyListener", string("caught inotify event: ") + eventToString( this_event->mask))

            if ((this_event->len > 0))
                    STRIGI_LOG_DEBUG ("strigi.InotifyListener", "event regards " + string(this_event->name, this_event->len))
            
            string file (iter->second);
            
            //TODO: fix path creation, check running OS
            if (file[file.length() - 1] != '/')
                file += '/';

            file += string(this_event->name, this_event->len);
            
            if ( (IN_MODIFY & this_event->mask) != 0 )
            {
                Event* event = new Event (Event::UPDATED, file);
                events.push_back (event);
            }
            else if (( (IN_DELETE & this_event->mask) != 0 ) || ( (IN_MOVED_FROM & this_event->mask) != 0 ) || ( (IN_DELETE_SELF & this_event->mask) != 0 ) || ( (IN_MOVE_SELF & this_event->mask) != 0 ))
            {
                if ( (IN_DELETE_SELF & this_event->mask) != 0 )
                    watchesToDel.insert (iter->first);

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

                    FileLister lister (m_filterManager);

                    lister.setCallbackFunction(&indexFileCallback);
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
                STRIGI_LOG_DEBUG ("strigi.InotifyListener", "inotify's unknown event")
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
        STRIGI_LOG_ERROR ("strigi.InotifyListener", message)
    }

    if (events.size() > 0)
        m_eventQueue->addEvents (events);

    // remove deleted watches
    set<unsigned int>::iterator i;
    for (i = watchesToDel.begin(); i != watchesToDel.end(); i++)
    {
        map < unsigned int, string>::iterator iter = m_watches.find(*i);
        m_watches.erase (iter);
    }

    fflush( NULL );
}

bool InotifyListener::isEventInteresting (struct inotify_event * event)
{
    // ignore files starting with '.'
    if (((IN_ISDIR & event->mask) == 0) && (event->len > 0) && ((event->name)[0] == '.'))
        return false;
    
    if (m_filterManager != NULL)
    {
        if ((event->len > 0) && m_filterManager->findMatch(event->name, event->len))
            return false;
    }
    else
        STRIGI_LOG_WARNING ("strigi.InotifyListener", "unable to use filters, m_filterManager == NULL!")

    return true;
}

bool InotifyListener::addWatch (const string& path)
{
    if (!m_bInitialized)
        return false;

    map<unsigned int, string>::iterator iter;
    for (iter = m_watches.begin(); iter != m_watches.end(); iter++)
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
            STRIGI_LOG_ERROR ("strigi.InotifyListener", "Failed to watch, maximum watch number reached")
            STRIGI_LOG_ERROR ("strigi.InotifyListener", "You've to increase the value stored into /proc/sys/fs/inotify/max_user_watches")
        }
        else if ( wd == -1 )
            STRIGI_LOG_ERROR ("strigi.InotifyListener", "Failed to watch " + path + " because of: " + strerror(-wd))
        else
        {
            char buff [20];
            snprintf(buff, 20* sizeof (char), "%i", wd);

            STRIGI_LOG_ERROR ("strigi.InotifyListener", "Failed to watch " + path + ": returned wd was " + buff + " (expected -1 or >0 )")
        }
        
        return false;
    }
    else
    {
        m_watches.insert(make_pair(wd, path));

        STRIGI_LOG_INFO ("strigi.InotifyListener", "added watch for " + path)
        
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
        
        m_pollingListener->addWatches( toPool);
    }
}

void InotifyListener::rmWatch(int wd, string path)
{
    if (inotify_rm_watch (m_iInotifyFD, wd) == -1)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener", "Error removing watch for " + path)
                STRIGI_LOG_ERROR ("strigi.InotifyListener", string("error: ") + strerror(errno))
    }
    else
    {
        char buff [20];
        snprintf(buff, 20 * sizeof (char), "%i",wd);
        STRIGI_LOG_DEBUG ("strigi.InotifyListener", string("Removed watch ") + buff + ": " + path)
    }
}

void InotifyListener::clearWatches ()
{
    for (map<unsigned int, string>::iterator iter = m_watches.begin(); iter != m_watches.end(); iter++)
        rmWatch (iter->first, iter->second);

    m_watches.clear();
}

void InotifyListener::dirRemoved (string dir, vector<Event*>& events)
{
    STRIGI_LOG_DEBUG ("strigi.InotifyListener", dir + " is no longer watched, removing all indexed files contained") 
    
    // we've to de-index all files contained into the deleted/moved directory
    if (m_pIndexReader) {
        // all indexed files
        map<string, time_t> indexedFiles = m_pIndexReader->getFiles( 0);
        
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
}

void InotifyListener::dirsRemoved (set<string> dirs, vector<Event*>& events)
{
    if (m_pIndexReader) {
        // all indexed files
        map<string, time_t> indexedFiles = m_pIndexReader->getFiles( 0);
        
        for (map<string, time_t>::iterator fileIt = indexedFiles.begin(); fileIt != indexedFiles.end(); fileIt++)
        {
            for (set<string>::iterator dirIt = dirs.begin(); dirIt != dirs.end(); dirIt++)
            {
                string::size_type pos = (fileIt->first).find (*dirIt);
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
}

void InotifyListener::setIndexedDirectories (const set<string> &dirs) {
    if (!m_pIndexReader) {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.setIndexedDirectories", "m_pIndexReader == NULL!")
        return;
    }
    if (m_eventQueue == NULL)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.setIndexedDirectories", "m_eventQueue == NULL!")
        return;
    }


    vector<Event*> events;
    set<string> newIndexedDirs, nomoreIndexedDirs;
    
    // search for new dirs to index
    for (set<string>::iterator iter = dirs.begin(); iter != dirs.end(); iter++)
    {
        set<string>::iterator it = m_indexedDirs.find(*iter);
        if (it == m_indexedDirs.end())
            newIndexedDirs.insert (*iter);
    }
    
    // search for dirs no more indexed
    for (set<string>::iterator iter = m_indexedDirs.begin(); iter != m_indexedDirs.end(); iter++)
    {
        set<string>::iterator it = dirs.find(*iter);
        if (it == dirs.end())
            nomoreIndexedDirs.insert (*iter);
    }
    
    char buff [20];
    snprintf(buff, 20 * sizeof (char), "%i",nomoreIndexedDirs.size());
    STRIGI_LOG_DEBUG ("strigi.InotifyListener.setIndexedDirectories", string(buff) + " dirs are no more indexed");
    
    snprintf(buff, 20 * sizeof (char), "%i",newIndexedDirs.size());
    STRIGI_LOG_DEBUG ("strigi.InotifyListener.setIndexedDirectories", string(buff) + " new dirs are to indexed");
    
    // TODO: add thread mutex
    
    // remove watches over no more indexed dirs
    for (set<string>::iterator it = nomoreIndexedDirs.begin(); it != nomoreIndexedDirs.end(); it++)
    {
        map<unsigned int, string>::iterator mi = m_watches.begin();
        while ( mi != m_watches.end())
        {
            if ((mi->second).find(*it) == 0)
            {
                // directory name begins with it
                rmWatch( mi->first, mi->second);
                map<unsigned int, string>::iterator rmIt = mi;
                mi++;
                m_watches.erase (rmIt);
            }
            else
                mi++;
        }
    }
    
    // remove old indexed files from db
    dirsRemoved (nomoreIndexedDirs, events);
    
    FileLister lister (m_filterManager);

    m_toWatch.clear();
    m_toIndex.clear();

    lister.setCallbackFunction(&indexFileCallback);
    lister.setDirCallbackFunction(&watchDirCallback);
    
    // walk over the newly added dirs
    for (set<string>::const_iterator iter = newIndexedDirs.begin(); iter != newIndexedDirs.end(); iter++)
        lister.listFiles(iter->c_str());

    for (map<string,time_t>::iterator iter = m_toIndex.begin(); iter != m_toIndex.end(); iter++)
    {
        Event* event = new Event (Event::CREATED, iter->first);
        events.push_back (event);
    }

    // add new watches
    addWatches (m_toWatch);
    
    m_toIndex.clear();
    m_toWatch.clear();

    if (events.size() > 0)
        m_eventQueue->addEvents (events);
    
    m_indexedDirs = dirs;
}

void InotifyListener::bootstrap (const set<string> &dirs) {
    if (!m_pIndexReader) {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.bootstrap", "m_pIndexReader == NULL!")
        return;
    }
    if (m_eventQueue == NULL)
    {
        STRIGI_LOG_ERROR ("strigi.InotifyListener.bootstrap", "m_eventQueue == NULL!")
        return;
    }
    
    vector<Event*> events;
    map <string, time_t> indexedFiles = m_pIndexReader->getFiles(0);
    
    FileLister lister (m_filterManager);

    m_toWatch.clear();
    m_toIndex.clear();

    lister.setCallbackFunction(&indexFileCallback);
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
            // file has NOT been changed since last run, we keep our indexed informations
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
        m_eventQueue->addEvents (events);
}

bool InotifyListener::ignoreFileCallback(const char* path, uint dirlen, uint len, time_t mtime)
{
    return true;
}

bool InotifyListener::indexFileCallback(const char* path, uint dirlen, uint len, time_t mtime)
{
    if (strstr(path, "/.")) return true;
    
    string file (path,len);

    (iListener->m_toIndex).insert (make_pair(file, mtime));

    return true;
}

void InotifyListener::watchDirCallback(const char* path, uint len)
{
    string dir (path, len);

    (iListener->m_toWatch).insert (dir);
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

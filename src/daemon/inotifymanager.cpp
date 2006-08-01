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
#include "inotifymanager.h"

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/select.h>
#include <cerrno>
#include <sys/resource.h>

#include "inotify/inotify.h"
#include "inotify/inotify-syscalls.h"

#include "filelister.h"
#include "indexreader.h"
#include "inotifyevent.h"
#include "inotifyeventqueue.h"

InotifyManager* manager;

using namespace std;
using namespace jstreams;

void* InotifyManagerStart (void *inotifymanager)
{
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
    static_cast<InotifyManager*>(inotifymanager)->run(0);
    pthread_exit(0);
}

InotifyManager::InotifyManager()
{
    manager = this;
    
    // listen only to interesting events
    m_iEvents = IN_CLOSE_WRITE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF;
    
    m_bMonitor = true;
    m_state = Idling;
    m_bInitialized = false;
    m_eventQueue = NULL;
    m_pIndexReader = NULL;
}

InotifyManager::~InotifyManager()
{
}

bool InotifyManager::init()
{
    m_iInotifyFD = inotify_init();
    if (m_iInotifyFD < 0)
    {
        cerr << "inotify_init() failed.  Are you running Linux 2.6.13 or later?\nIf so, something mysterious has gone wrong.\n";
        return false;
    }
    
    m_bInitialized = true;
    
    printf ("InotifyManager successfully initialized\n");
    
    return true;
}

bool InotifyManager::start()
{
    if (!m_bInitialized)
    {
        if (!init())
            return false;
    }
    
    // start the inotify thread
    int ret = pthread_create(&m_thread, NULL, InotifyManagerStart, this);
    if (ret < 0)
    {
        printf("cannot create thread\n");
        return false;
    }
    
    return true;
}

void* InotifyManager::run(void*)
{
    while (m_state != Stopping)
    {
        watch();
        
        if (m_state == Watching)
            m_state = Idling;
    }
    
    return 0;
}

void InotifyManager::stop()
{
    m_state = Stopping;
    
    if (m_thread)
    {
        // wait for the indexer to finish
        pthread_join(m_thread, 0);
    }
    
    m_thread = 0;
}

void InotifyManager::watch ()
{
    // some code taken from inotify-tools (http://inotify-tools.sourceforge.net/)
    
    vector <InotifyEvent*> events;
    set <unsigned int> watchesToDel;
    
    struct timeval read_timeout;
    read_timeout.tv_sec = 10;
    read_timeout.tv_usec = 0;
    
    static const int MAX_EVENTS = 4096;
    struct inotify_event event[MAX_EVENTS];
    event[0].wd = 0;
    event[0].mask = 0;
    event[0].cookie = 0;
    event[0].len = 0;
    
    ssize_t bytes = 0;
    fd_set read_fds;
    
    bytes = 0;
    
    while ( bytes < sizeof(struct inotify_event) )
    {
    
        FD_ZERO(&read_fds);
        FD_SET(m_iInotifyFD, &read_fds);
        
        int rc = select(m_iInotifyFD + 1, &read_fds, NULL, NULL, &read_timeout);
        
        if ( rc < 0 )
        {
            cerr << "Select on inotify failed\n";
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
            cerr << "Read from inotify failed\n";
            return;
        }
        
        if ( thisBytes == 0 )
        {
            cerr << "Inotify reported end-of-file.  Possibly too many events occurred at once.\n";
            return;
        }
        
        bytes += thisBytes;
    }
    
    map < unsigned int, string>::iterator iter;
    
    static struct inotify_event * this_event;
    
    static char * this_event_char;
    this_event_char = (char *)&event[0];
    
    static int remaining_bytes;
    remaining_bytes = bytes;
    
    do
    {
//         string this_event_str;
        
        this_event = (struct inotify_event *)this_event_char;
        
        iter = m_watches.find (this_event->wd);
    
        if (iter == m_watches.end())
        {
            cerr << "inotify: returned an unknown watch descriptor\n";
            continue;
        }
        
        if (!(((IN_ISDIR & this_event->mask) == 0) && (this_event->len > 0) && ((this_event->name)[0] == '.')))
        // we ignore every action on a file starting with '.'
        {
//             cout << "inotify: " << iter->second << " changed\n";
            
//             this_event_str = eventToString( this_event->mask);
//             cout << "event str: |" << this_event_str << "|\n";
        
//             if ((this_event->len > 0))
//                 printf("event name |%s|\n", this_event->name );
            
            //TODO: fix path creation
            string file = iter->second;
            
            if (file[file.length() - 1 ] != '/')
                file += "/";
            
            file += this_event->name;
            
            if ( (IN_MODIFY & this_event->mask) != 0 )
            {
                InotifyEvent* event = new InotifyEvent (InotifyEvent::UPDATED, file, this_event->wd, time (NULL));
                events.push_back (event);
            }
            else if (( (IN_DELETE & this_event->mask) != 0 ) || ( (IN_MOVED_FROM & this_event->mask) != 0 ) || ( (IN_DELETE_SELF & this_event->mask) != 0 ) || ( (IN_MOVE_SELF & this_event->mask) != 0 ))
            {
                if ( (IN_DELETE_SELF & this_event->mask) != 0 )
                    watchesToDel.insert (iter->first);
                
                if ((IN_ISDIR & this_event->mask) != 0)
                {   
                    // we've to de-index all files contained into the deleted/moved directory
                    if (m_pIndexReader != NULL)
                    {
                        // all indexed files
                        map<string, time_t> indexedFiles = m_pIndexReader->getFiles( 0);
                        
                         for (map<string, time_t>::iterator it = indexedFiles.begin(); it != indexedFiles.end(); it++)
                        {
                            string::size_type pos = (it->first).find (file);
                            if (pos == 0)
                            {
                                InotifyEvent* event = new InotifyEvent (InotifyEvent::DELETED, it->first, this_event->wd, time (NULL));
                                events.push_back (event);
                            }
                        }
                    }
                    else
                        cout << "InotifyManager:: m_pIndexReader == NULL!\n";
                }
                else
                {
                    InotifyEvent* event = new InotifyEvent (InotifyEvent::DELETED, file, this_event->wd, time (NULL));
                    events.push_back (event);
                }
            }
            else if ( (IN_CLOSE_WRITE & this_event->mask) != 0 )
            {
                InotifyEvent* event = new InotifyEvent (InotifyEvent::UPDATED, file, this_event->wd, time (NULL));
                events.push_back (event);
            }
            else if (( (IN_CREATE & this_event->mask) != 0 ) || ( (IN_MOVED_TO & this_event->mask) != 0 ))
            {
                if ( (IN_ISDIR & this_event->mask) != 0 )
                {
                    // a new directory has been created / moved into a watched place
                    
                    m_toIndex.clear();
                    
                    FileLister lister;
    
                    lister.setCallbackFunction(&indexFileCallback);
                    lister.setDirCallbackFunction(&watchDirCallback);
    
                    lister.listFiles(file.c_str());
                    
                    for (set<string>::iterator i = m_toIndex.begin(); i != m_toIndex.end(); i++)
                    {
                        InotifyEvent* event = new InotifyEvent (InotifyEvent::CREATED, *i, this_event->wd, time (NULL));
                        events.push_back (event);
                    }
                    
                    m_toIndex.clear();
                }
                else
                {
                    InotifyEvent* event = new InotifyEvent (InotifyEvent::CREATED, file, this_event->wd, time (NULL));
                    events.push_back (event);
                }
            }
            else
                 cout << "inotify: event unknown\n";
        }

        this_event_char += sizeof(struct inotify_event) + this_event->len;
        remaining_bytes -= sizeof(struct inotify_event) + this_event->len;
    }
    while (remaining_bytes >= sizeof(struct inotify_event) );

    // I _think_ this should never happen.
    if (remaining_bytes != 0 ) {
        fprintf(stderr,
                "%f event(s) may have been lost!\n",
                ((float)remaining_bytes)/((float)sizeof(struct inotify_event))
                );
    }
    
    if (events.size() > 0)
    {
        if (m_eventQueue == NULL)
            cerr << "InotifyManager: m_eventQueue == NULL!\n";
        else
            m_eventQueue->addEvents (events);
    }
    
    // remove deleted watches
    for (set<unsigned int>::iterator i = watchesToDel.begin(); i != watchesToDel.end(); i++)
    {
        map < unsigned int, string>::iterator iter = m_watches.find(*i);
        m_watches.erase (iter);
    }
    
    fflush( NULL );
}

void InotifyManager::addWatch (const string path)
{
    if (!m_bInitialized)
        return;
    
    static int wd;
    wd = inotify_add_watch (m_iInotifyFD, path.c_str(), m_iEvents);
    
    if (wd < 0)
    {
        if ( wd == -1 )
        {
            cerr << "Failed to watch "<< path<< "because of: " << strerror(-wd)<<endl;
        }
        else
        {
            cerr << "Failed to watch " << path <<": returned wd was "<<wd<<" (expected -1 or >0 )\n";
        }
    }
    else
    {
        m_watches.insert(make_pair(wd, path));
        
        cout << "inotify: added watch for " << path << endl;
    }
}

void InotifyManager::addWatches (const set<string> &watches)
{
    set<string>::iterator iter;
    
    for (iter = watches.begin(); iter != watches.end(); iter++)
        addWatch (*iter);
}

void InotifyManager::setIndexedDirectories (const set<string> &dirs)
{
    FileLister lister;
    
    lister.setCallbackFunction(&ignoreFileCallback);
    lister.setDirCallbackFunction(&watchDirCallback);
    
    set<string>::const_iterator iter;
    for (iter = dirs.begin(); iter != dirs.end(); iter++)
        lister.listFiles(iter->c_str());
}

bool InotifyManager::ignoreFileCallback(const char* path, uint dirlen, uint len, time_t mtime)
{
    return true;
}

bool InotifyManager::indexFileCallback(const char* path, uint dirlen, uint len, time_t mtime)
{
    if (strstr(path, "/.")) return true;
    
    (manager->m_toIndex).insert (string(path));
    
    return true;
}

void InotifyManager::watchDirCallback(const char* path)
{
    manager->addWatch( string (path));
}

string InotifyManager::eventToString(int events)
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

bool InotifyManager::isRelevant (struct inotify_event * event)
{
    if ((IN_CLOSE_WRITE & event->mask) != 0)
        return true;
    else if ( (IN_MODIFY & event->mask) != 0 )
        return true;
    else if ((IN_MOVED_FROM & event->mask) != 0)
        return true;
    else if ((IN_MOVED_TO & event->mask) != 0)
        return true;
    else if ((IN_CREATE & event->mask) != 0)
        return true;
    else if ((IN_DELETE & event->mask) != 0)
        return true;
    else if ((IN_DELETE_SELF & event->mask) != 0)
        return true;
    else if ((IN_MOVE_SELF & event->mask) != 0)
        return true;
    else
        return false;
}

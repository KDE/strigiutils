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
#include "inotifyeventqueue.h"
#include "inotifyevent.h"
#include <algorithm>

#include <iostream>

using namespace std;

InotifyEventQueue::InotifyEventQueue()
{
    pthread_mutex_init (&m_mutex, 0);
}

InotifyEventQueue::~InotifyEventQueue()
{
    pthread_mutex_destroy (&m_mutex);
    
    clear();
}

void InotifyEventQueue::clear()
{
    clearVector();
    clearMap();
}

void InotifyEventQueue::clearVector()
{
    vector<InotifyEvent*>::iterator j;
        
    for (j = m_optEvents.begin(); j != m_optEvents.end(); j++)
        delete *j;
    
    m_optEvents.clear();
}

void InotifyEventQueue::clearMap()
{
    map <pair <int, string>, vector<InotifyEvent*> >::iterator iter;
    vector<InotifyEvent*>::iterator j;
        
    for (iter = m_events.begin(); iter != m_events.end(); iter++)
    {
        for (j = (iter->second).begin(); j != (iter->second).end(); j++)
            delete *j;
    }
    
    m_events.clear();
}

void InotifyEventQueue::addEvents (vector<InotifyEvent*> events)
{
    vector<InotifyEvent*>::iterator iter;
    
    pthread_mutex_lock (&m_mutex);
    
    for (iter = events.begin(); iter != events.end(); iter++)
    {
        InotifyEvent* event = *iter;
        
        m_events[event->hash()].push_back(event);
    }
    
    pthread_mutex_unlock (&m_mutex);
}

vector<string> InotifyEventQueue::getEventStrings (unsigned char type)
{
    vector<string> strings;
    vector<InotifyEvent*>::iterator iter;
    
    for (iter = m_optEvents.begin(); iter != m_optEvents.end(); iter++)
    {
        InotifyEvent* event = *iter;
        
        if ((event->getType() & type) != 0)
            strings.push_back (event->getPath());
    }
    
    return strings;
}

vector <InotifyEvent*> InotifyEventQueue::getEvents()
{
    vector <InotifyEvent*> result;
    
    if (pthread_mutex_trylock (&m_mutex))
    {
        optimize();
        result = m_optEvents;
        m_optEvents.clear();
        
        pthread_mutex_unlock (&m_mutex);
    }
    
    return result;
}

void InotifyEventQueue::optimize()
{
    map <pair <int, string>, vector<InotifyEvent*> >::iterator iter;
    unsigned int totalEvents = 0;
    
    clearVector();
    
    for (iter = m_events.begin(); iter != m_events.end(); iter++)
    {
        vector<InotifyEvent*> v = iter->second;
        
        totalEvents += v.size();
        
        if (v.size() == 0)
            continue; // should never happen
        
        InotifyEvent* event = v[0];
        
        // delete , create --> update
        // delete. update --> update
        // create, delete --> NOTHING
        // create, update --> create
        // update, delete --> delete
        // update, create --> update
        for (unsigned int i = 1; i < v.size(); i++)
        {
            if (event == NULL)
            {
                event = v[i];
                continue;
            }
            else if (event->getType() == v[i]->getType())
                continue;
            
            switch (event->getType())
            {
                case InotifyEvent::CREATED:
                    if (v[i]->getType() == InotifyEvent::DELETED)
                        event = NULL;
                    break;
                case InotifyEvent::DELETED:
                    event->setType( InotifyEvent::UPDATED);
                    break;
                case InotifyEvent::UPDATED:
                    if (v[i]->getType() == InotifyEvent::DELETED)
                        event = v[i];
                    break;
            }
        }
        
        if (event != NULL)
            m_optEvents.push_back (new InotifyEvent (event));
    }
    
//     printf ("optimized events:\n");
//     for (std::vector<InotifyEvent*>::iterator it = m_optEvents.begin(); it != m_optEvents.end(); it++)
//     {
//         printf ("event\n");
//         cout << *it;
//     }
    
    clearMap();
}

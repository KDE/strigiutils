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

#include "event.h"
#include "eventlistenerqueue.h"
#include "strigi_thread.h"

#include <algorithm>
#include <iostream>

using namespace std;

EventListenerQueue::EventListenerQueue()
{
    STRIGI_MUTEX_INIT (&m_mutex);
}

EventListenerQueue::~EventListenerQueue()
{
    STRIGI_MUTEX_DESTROY (&m_mutex);

    clear();
}

void EventListenerQueue::clear()
{
    map < string, Event*>:: iterator iter;

    for (iter = m_events.begin(); iter != m_events.end(); iter++)
    {
        delete iter->second;
    }

    m_events.clear();
}

void EventListenerQueue::addEvents (vector<Event*> events)
{
    vector<Event*>::iterator iter;
    map < string, Event*>::iterator mapIt;

    STRIGI_MUTEX_LOCK (&m_mutex);

    for (iter = events.begin(); iter != events.end(); iter++)
    {
        Event* event = *iter;

        mapIt = m_events.find(event->getPath());

        if (mapIt != m_events.end())
            delete mapIt->second;

        m_events[event->getPath()] = event;
    }

    STRIGI_MUTEX_UNLOCK (&m_mutex);
}

vector <Event*> EventListenerQueue::getEvents()
{
    vector <Event*> result;

    if (STRIGI_MUTEX_TRY_LOCK (&m_mutex))
    {
        for (map<string, Event*>::iterator iter = m_events.begin(); iter != m_events.end(); iter++)
        {
            result.push_back (iter->second);
        }

        m_events.clear();

        STRIGI_MUTEX_UNLOCK (&m_mutex);
    }

    return result;
}

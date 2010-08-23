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

#include "eventlistenerqueue.h"
#include "event.h"
#include "strigi/strigi_thread.h"
#include "strigilogging.h"

#include <algorithm>
#include <iostream>
#include <stdio.h>

#if defined(__SUNPRO_CC)
#include <unistd.h>
#endif

using namespace std;

EventListenerQueue::EventListenerQueue(const char* name)
    : StrigiThread (name)
{
    STRIGI_MUTEX_INIT (&m_mutex);
    m_toProcessOldCount = 0;
    m_processedOldCount = 0;
    m_waitingOldCount = 0;
}

EventListenerQueue::~EventListenerQueue()
{
    clear();
    
    STRIGI_MUTEX_DESTROY (&m_mutex);
}

void EventListenerQueue::clearEvents( map<string, Event*>& events)
{
    map < string, Event*>:: iterator iter;

    for (iter = events.begin(); iter != events.end(); ++iter)
    {
        delete iter->second;
    }
    
    events.clear();
}

void EventListenerQueue::clear()
{
    STRIGI_MUTEX_LOCK (&m_mutex);
    
    clearEvents (m_toProcess);
    clearEvents (m_processed);
    clearEvents (m_waiting);
    
    STRIGI_MUTEX_UNLOCK (&m_mutex);
}

void* EventListenerQueue::run(void*)
{
    while (getState() != Stopping)
    {
        sleep (5);
        if (STRIGI_MUTEX_TRY_LOCK(&m_mutex) == 0)
        {
            
            if (m_waiting.size() > 0)
            {
                // search all wainting events that can be promoted to
                // "toProcess" status
                
                map<string,Event*>::iterator iter = m_waiting.begin();
                
                time_t now;
                time (&now);
                double delta = 60;
                
                while (iter != m_waiting.end())
                {
                    if (checkElapsedTime(iter->second, now, delta))
                    {
                        m_toProcess.insert(make_pair(iter->first,iter->second));
                        map<string,Event*>::iterator itRm = iter;
                        iter++;
                        m_waiting.erase(itRm);
                    }
                    else
                        iter++;
                }
            }
            
            if (m_processed.size() > 0)
                purgeProcessedEvents();

            // print stats
            char buff [50];

            if (m_processed.size() != m_processedOldCount) {
                snprintf (buff, 50*sizeof(char), "%lu", (unsigned long)m_processed.size());
                STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.run",
                            string(buff) + " events in \"processed\" queue");
                m_processedOldCount = (unsigned int)m_processed.size();
            }

            if (m_toProcess.size() != m_toProcessOldCount) {
                snprintf (buff, 50*sizeof(char), "%lu", (unsigned long)m_toProcess.size());
                STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.run",
                            string(buff) + " events in \"to process\" queue");
                m_toProcessOldCount = (unsigned int)m_toProcess.size();
            }

            if (m_waiting.size() != m_waitingOldCount) {
                snprintf (buff, 50*sizeof(char), "%lu", (unsigned long)m_waiting.size());
                STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.run",
                                string(buff) + " events in \"waiting\" queue");
                m_waitingOldCount = (unsigned int)m_waiting.size();
            }
            
            STRIGI_MUTEX_UNLOCK (&m_mutex);
        }

        if (getState() == Working)
            setState(Idling);
    }

    STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.run",
                       string("exit state: ") + getStringState());
    return 0;
}

bool EventListenerQueue::checkElapsedTime (Event* event,time_t now,double delta)
{
    double diff = difftime (now, event->getTime());
    
    if (diff > delta)
        return true;
    
    return false;
}

void EventListenerQueue::purgeProcessedEvents()
{
    unsigned int counter = 0;
    time_t now;
    time (&now);
    
    double delta = 120;
    
    map <string, Event*>::iterator iter = m_processed.begin();
    
    while (iter != m_processed.end())
    {
        if (checkElapsedTime(iter->second, now, delta))
        {
            map <string, Event*>::iterator itRm = iter;
            iter++;
            
            // we remove this event forever, free-up some memory
            delete itRm->second;
            itRm->second = NULL;
            m_processed.erase (itRm);
            counter++;
        }
        else
            iter++;
    }
    
    if (counter > 0)
    {
        char buff [50];
        snprintf (buff, 50*sizeof (char), "%i", counter);
        
        STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.purgeProcessedEvents",
                        string(buff) + " has been removed from processed " +
                        "events queue");
    }
}

Event* EventListenerQueue::searchInProcessed(Event* event)
{
    if (event == NULL)
        return NULL;

    map<string, Event*>::iterator iter;
    iter = m_processed.find(event->getPath());
    
    if (iter == m_processed.end())
        return NULL;
    else
        return iter->second;
}

Event* EventListenerQueue::searchInWaiting (Event* event)
{
    map<string, Event*>::iterator iter;
    iter = m_waiting.find(event->getPath());
    
    if (iter == m_waiting.end())
        return NULL;
    else
        return iter->second;
}

Event* EventListenerQueue::searchInToProcess (Event* event)
{
    map<string, Event*>::iterator iter;
    iter = m_toProcess.find(event->getPath());
    
    if (iter == m_toProcess.end())
        return NULL;
    else
        return iter->second;
}

void EventListenerQueue::putInWaiting (Event* event)
{
    Event* waitingEvent = searchInWaiting(event);
    
    if (waitingEvent)
    {
        // there's already an event regarding this file/dir
        // let's update the event type
        updateEventType(waitingEvent, event, m_waiting);
        
        // we don't need event anymore, free-up some memory
        delete event;
        event = NULL;
        
        STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.addEvents",
                          "updated waiting event status");
    }
    else
    {
        // add the new event to the wainting queue
        m_waiting.insert (make_pair(event->getPath(), event));
        STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.putInWaiting",
                          "new event added to waiting queue");
    }
}

void EventListenerQueue::putInProcessed (Event* event)
{
    Event* processedEvent = searchInProcessed(event);
    
    if (processedEvent)
    {
        // there's already an event regarding this file/dir
        // let's update this one
        processedEvent->setType(event->getType());
        processedEvent->setTime(event->getTime());
        
        // we don't need event anymore, free-up some memory
        delete event;
        event = NULL;
        
        STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.putInProcessed",
                          "updated processed event");
    }
    else
    {
        // add the new event to the wainting queue
        m_processed.insert (make_pair(event->getPath(), event));
        STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.putInProcessed",
                          "new event added to processed queue");
    }
}

void EventListenerQueue::updateEventType (Event* oldEvent, Event* newEvent,
                                          map <string, Event*>& eventMap)
{
    // simplification table
    // old event | new event |   result 
    //     C     |     C     | IMPOSSIBLE
    //     C     |     U     |     C
    //     C     |     D     |   REMOVE
    //     U     |     U     |     U
    //     U     |     C     | IMPOSSIBLE
    //     U     |     D     |     D
    //     D     |     D     | IMPOSSIBLE
    //     D     |     U     | IMPOSSIBLE
    //     D     |     C     |     U
    
    /*STRIGI_LOG_DEBUG ("strigi.EventListener.Queue.updateEventType",
                      "old event: " + oldEvent->toString())
    STRIGI_LOG_DEBUG ("strigi.EventListener.Queue.updateEventType",
                      "new event: " + newEvent->toString())*/
    
    switch (oldEvent->getType())
    {
        case Event::CREATED:
        {
            switch (newEvent->getType())
            {
                case Event::CREATED:
                    STRIGI_LOG_INFO("strigi.EventListenerQueue.updateEventType",
                                    "CREATED --> CREATED - "
                                    "Maybe we have lost some filesystem event");
                    // put in update state, it's the safer solution because with
                    // update event we make a delete and a create
                    oldEvent->setType( Event::UPDATED);
                    break;
                case Event::UPDATED:
                    STRIGI_LOG_ERROR("strigi.EventListenerQueue.updateEventType",
                                     "CREATED --> UPDATED = leave CREATED");
                    // leave CREATED state
                    break;
                case Event::DELETED:
                    // remove the old event, the new one makes it useless
                    eventMap.erase(oldEvent->getPath());
                    
                    // deallocate event
                    delete oldEvent;
                    oldEvent = NULL;
                    break;
            }
            break;
        }
        case Event::UPDATED:
        {
            switch (newEvent->getType())
            {
                case Event::CREATED:
                    STRIGI_LOG_INFO("strigi.EventListenerQueue.updateEventType",
                                    "UPDATED --> CREATED - "
                                    "Maybe we have lost some filesystem event");
                    // leave UPDATED state, it's the safer solution because with
                    // update event we make a delete and a create
                    break;
                case Event::UPDATED:
                    // leave UPDATED state
                    STRIGI_LOG_ERROR("strigi.EventListenerQueue.updateEventType",
                                     "UPDATED --> UPDATED = leave UPDATED");

                    break;
                case Event::DELETED:
                    oldEvent->setType( Event::DELETED);
                    STRIGI_LOG_ERROR("strigi.EventListenerQueue.updateEventType",
                                     "UPDATED --> DELETED = DELETED");
                    break;
            }
            break;
        }
        case Event::DELETED:
        {
            switch (newEvent->getType())
            {
                case Event::CREATED:
                    oldEvent->setType( Event::UPDATED);
                    STRIGI_LOG_ERROR("strigi.EventListenerQueue.updateEventType",
                                     "DELETED --> CREATED = UPDATED");
                    break;
                case Event::UPDATED:
                    STRIGI_LOG_INFO("strigi.EventListenerQueue.updateEventType",
                                     "DELETED --> UPDATED - "
                                     "Maybe we have lost some filesystem event");
                    
                    // put in update state, it's the safer solution because with
                    // update event we make a delete and a create
                    oldEvent->setType( Event::UPDATED);
                    break;
                case Event::DELETED:
                    STRIGI_LOG_INFO("strigi.EventListenerQueue.updateEventType",
                                    "DELETED -> DELETED - "
                                    "Maybe we have lost some filesystem event");
                    //leave DELETED state
                    break;
            }
            break;
        }
    }
}

void EventListenerQueue::addEvents (vector<Event*> events)
{
    vector<Event*>::iterator iter;

    STRIGI_MUTEX_LOCK (&m_mutex);
    
    Event* event = NULL;

    for (iter = events.begin(); iter != events.end(); ++iter)
    {
        event = searchInProcessed(*iter);
        if (event)
        {
            // not to much time ago we have processed an event regarding
            // the same file/dir, we have to wait a little before processing this
            // new event
            putInWaiting (*iter);
            continue;
        }
        
        event = searchInWaiting(*iter);
        if (event)
        {
            // there's another event in the wainting queue that is related with
            // this file/dir. Let's update its status
            updateEventType (event, *iter, m_waiting);
            
            // we don't need this new event anymore, free-up some memory
            delete *iter;
            *iter = NULL;
            
            continue;
        }
        
        event = searchInToProcess(*iter);
        if (event)
        {
            updateEventType (event, *iter, m_toProcess);
            
            // we don't need this new event anymore, free-up some memory
            delete *iter;
            *iter = NULL;
            
            continue;
        }
        else
        {
            Event* e = *iter;
            string temp;

            // if we're arrived here this new event has to be processed
            m_toProcess.insert (make_pair (e->getPath(), e));
            STRIGI_LOG_DEBUG ("strigi.EventListenerQueue.addEvents",
                              string("event regarding ") + e->getPath() +
                                      " has been put in toProcess queue");
        }
    }

    STRIGI_MUTEX_UNLOCK (&m_mutex);
}

unsigned int EventListenerQueue::size()
{
    unsigned int i = 0;
    
    if (STRIGI_MUTEX_TRY_LOCK (&m_mutex) == 0)
    {
        i = (unsigned int)m_toProcess.size();
        STRIGI_MUTEX_UNLOCK (&m_mutex);
    }
    
    return i;
}

vector <Event*> EventListenerQueue::getEvents()
{
    vector <Event*> result;

    if (STRIGI_MUTEX_TRY_LOCK (&m_mutex) == 0)
    {
        for (map<string, Event*>::iterator iter = m_toProcess.begin();
             iter != m_toProcess.end(); ++iter)
        {
            result.push_back (new Event (iter->second));
            
            // move event to processed queue
            putInProcessed (iter->second);
        }

        m_toProcess.clear();

        STRIGI_MUTEX_UNLOCK (&m_mutex);
    }

    return result;
}

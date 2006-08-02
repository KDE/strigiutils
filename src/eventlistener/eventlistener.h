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
#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <set>
#include <string>
#include <pthread.h>

class EventListenerQueue;

class EventListener
{
    public:
        EventListener() { m_state = Idling; }

        ~EventListener() {};
        
        virtual void addWatch (const std::string path) = 0;
        virtual void addWatches (const std::set<std::string> &watches) = 0;
        virtual void setIndexedDirectories (const std::set<std::string> &dirs) = 0;
        void setEventListenerQueue (EventListenerQueue* eventQueue) { m_eventQueue = eventQueue; }
        
        virtual bool start() = 0;
        virtual void* run(void*) = 0;
        virtual void stop() = 0;
        
    protected:
        enum State {Idling, Watching, Stopping};
        State m_state;

        EventListenerQueue* m_eventQueue;
        pthread_t m_thread;
};

#endif

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
#ifndef INOTIFYEVENTQUEUE_H
#define INOTIFYEVENTQUEUE_H

#include <map>
#include <vector>
#include <pthread.h>

class InotifyEvent;
class InotifyManager;
class IndexScheduler;

class InotifyEventQueue
{
    public:
        InotifyEventQueue();
        ~InotifyEventQueue();
        
        void addEvents (std::vector<InotifyEvent*> events);
        unsigned int size() { return m_events.size(); }
        std::vector<std::string> getEventStrings (unsigned char type);
        std::vector <InotifyEvent*> getEvents();
        
        friend class InotifyManager;
        
    protected:
        void optimize();
        void clear();
        void clearMap();
        void clearVector();
        
        std::map <std::pair <int, std::string>, std::vector <InotifyEvent*> > m_events;
        std::vector <InotifyEvent*> m_optEvents;
        pthread_mutex_t m_mutex;
};

#endif

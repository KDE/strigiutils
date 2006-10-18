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
#ifndef EVENTLISTENERQUEUE_H
#define EVENTLISTENERQUEUE_H

#include <map>
#include <vector>
#include <pthread.h>

class Event;

/*!
* @class EventListenerQueue
* @brief A collector of all event instances
*
* All Event occurrencies are stored here. Interested classes can retrieve them using getEvents method.
*
* In a producer-consumer pattern EventListenerQueue is the place where all producer store the products
*/
class EventListenerQueue
{
    public:
        explicit EventListenerQueue();
        ~EventListenerQueue();
        
        /*!
        * @param events a vector containing new Events to manage
        */
        void addEvents (std::vector<Event*> events);
        
        /*!
        * @return the number of events availables
        */
        unsigned int size() { return m_events.size(); }
        
        /*!
        * @return a vector containing all events, after that m_events is cleared
        */
        std::vector <Event*> getEvents();
       
    protected:
        /*!
        * deallocate all events stored into m_events. In the end m_events will be empty.
        */
        void clear();
       
        std::map <std::string, Event*> m_events; //!< all event instances
        pthread_mutex_t m_mutex;//!< mutex lock over m_events
};

#endif

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
#include "strigithread.h"

class EventListenerQueue;
class FilterManager;

namespace jstreams
{
    class IndexReader;
}

/*!
* @class EventListener
* @brief parent of all listener class
*/

class EventListener : public StrigiThread {
public:
    explicit EventListener(const char* name) :StrigiThread(name) {
        m_pEventQueue = NULL;
        m_pFilterManager = NULL;
        m_pIndexReader = NULL;
        m_pollingInterval = 180;
    }

    virtual ~EventListener() {};

    virtual bool init() = 0;
    virtual bool addWatch (const std::string& path) = 0;
    virtual void addWatches (const std::set<std::string>& watches) = 0;
    virtual void setIndexedDirectories (const std::set<std::string>& dirs) = 0;
    void setEventListenerQueue (EventListenerQueue* eventQueue) {
        m_pEventQueue = eventQueue;
    }
    void setFilterManager (FilterManager* filterManager) {
        m_pFilterManager = filterManager;
    }
    void setIndexReader (jstreams::IndexReader* ireader) {
        m_pIndexReader = ireader;
    }
    unsigned int getPollingInterval() {
        return m_pollingInterval;
    }
    /*! 
    * configure the polling interval. The time elapsed between each polling
    * operation (expressed in seconds) 
    */
    void setPollingInterval(unsigned int value) {
        m_pollingInterval = value;
    }

protected:
    EventListenerQueue* m_pEventQueue;
    FilterManager* m_pFilterManager;
    jstreams::IndexReader* m_pIndexReader;
    unsigned int m_pollingInterval;//!< pause time between each polling operation
};

#endif

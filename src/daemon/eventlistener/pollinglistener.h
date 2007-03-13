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
#ifndef POLLINGLISTENER_H
#define POLLINGLISTENER_H

#include "eventlistener.h"

#include <map>
#include <pthread.h>

class Event;

/**
* @class PollingListener
* @brief A simple class that polls periodically the filesystem looking for changes
*
* It's used when InotifyListener reaches max user watches limit, all remaining dirs to watch are checked by PollingListener.
* Update interval is configurable by the user.
* @sa Filter
*/

class PollingListener : public EventListener {
public:
    PollingListener();
    PollingListener(std::set<std::string>& dirs);

    ~PollingListener();

    bool addWatch (const std::string& path);
    void rmWatch (const std::string& path);
    void addWatches (const std::set<std::string>& watches);
    void setIndexedDirectories (const std::set<std::string>& dirs);

    void* run(void*);

    static void fileCallback(const char* path, unsigned dirlen, unsigned len,
        time_t mtime);
private:
    void poll ();
    void clearWatches();

    bool m_firstTime;
    std::set<std::string> m_watches;
    std::map<std::string, time_t> m_toIndex;
    pthread_mutex_t m_mutex; //!< mutex on m_watches
    static PollingListener* workingPoller; //!<pointer to current PollingListener instance, used with FileLister's callbacks
};

#endif

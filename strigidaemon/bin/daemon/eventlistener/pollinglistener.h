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
#include <strigi/diranalyzer.h>

#include <map>
#include "strigi/strigi_thread.h"

class Event;

/**
* @class PollingListener
* @brief A simple class that periodically polls the filesystem looking for
* changes.
*
* The polling listener walks through all the subdirectories of the set
* directories and reports on files that have been modified since the last time
* the polling was performed.
*
* This class is used when inotify is not available on a system or when
* InotifyListener reaches max user watches limit. All the remaining directories
* that need to be watched are checked by PollingListener.
*
* The update interval is configurable by the user.
* @sa Filter
*/
class PollingListener : public EventListener, Strigi::AnalysisCaller {
public:
    PollingListener();
    PollingListener(std::set<std::string>& dirs);

    ~PollingListener();

    bool addWatch (const std::string& path);
    void rmWatch (const std::string& path);
    void addWatches (const std::set<std::string>& watches);
    void setIndexedDirectories (const std::set<std::string>& dirs);

    void* run(void*);

private:
    void poll ();
    void clearWatches();

    std::vector<std::string> m_watches;
    STRIGI_MUTEX_DEFINE(m_mutex); //!< mutex on m_watches

    bool continueAnalysis();
};

#endif

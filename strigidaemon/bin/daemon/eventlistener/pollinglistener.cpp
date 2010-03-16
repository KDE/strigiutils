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
#include "pollinglistener.h"

#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>
#include <cassert>

#include <strigi/diranalyzer.h>
#include "combinedindexmanager.h"
#include "event.h"
#include "eventlistenerqueue.h"
#include <strigi/filelister.h>
#include <strigi/indexreader.h>
#include "../strigilogging.h"
#include "strigi/strigi_thread.h"
#include <unistd.h>

using namespace std;
using namespace Strigi;

namespace {
    /*!
    * @param path string containing path to check
    * Appends the terminating char to path.
    * Under Windows that char is '\', '/' under *nix
    */
    string fixPath (string path)
    {
        if ( path.c_str() == NULL || path.length() == 0 )
            return "";

        string temp(path);

    #ifdef HAVE_WINDOWS_H
        size_t l= temp.length();
        char* t = (char*)temp.c_str();
        for (size_t i=0;i<l;i++){
            if ( t[i] == '\\' )
                t[i] = '/';
        }
        temp[0] = tolower(temp.at(0));
    #endif

        char separator = '/';

        if (temp[temp.length() - 1 ] != separator)
            temp += separator;

        return temp;
    }
}

PollingListener::PollingListener() :EventListener("PollingListener") {
    setState(Idling);
    STRIGI_MUTEX_INIT(&m_mutex);
}
PollingListener::PollingListener(set<string>& dirs)
        :EventListener("PollingListener") {
    setState(Idling);
    STRIGI_MUTEX_INIT(&m_mutex);

    addWatches(dirs);
}
PollingListener::~PollingListener() {
    m_watches.clear();
    STRIGI_MUTEX_DESTROY(&m_mutex);
}
void*
PollingListener::run(void*) {
    STRIGI_LOG_DEBUG ("strigi.PollingListener.run", "started");

    while (getState() != Stopping) {
        // wait m_pause seconds before polling again
        sleep(m_pollingInterval);
        if (getState() != Stopping) {
            poll();
        }
        if (getState() == Working) {
            setState(Idling);
        }
    }

    STRIGI_LOG_DEBUG ("strigi.PollingListener.run",
        string("exit state: ") + getStringState());
    return 0;
}
void PollingListener::poll () {
    assert(m_pManager);
    assert(m_pAnalyzerConfiguration);

    // get a shadow copy of m_watches
    STRIGI_MUTEX_LOCK (&m_mutex);
    vector<string> watches = m_watches;
    STRIGI_MUTEX_UNLOCK (&m_mutex);

    DirAnalyzer diranalyzer(*m_pManager, *m_pAnalyzerConfiguration);
    STRIGI_LOG_DEBUG ("strigi.PollingListener.poll", "going across filesystem");
    diranalyzer.updateDirs(watches, 1, this);
    STRIGI_LOG_DEBUG ("strigi.PollingListener.poll",
                      "filesystem access finished");
}

bool
PollingListener::addWatch (const string& path) {
    STRIGI_MUTEX_LOCK (&m_mutex);

    vector<string>::const_iterator i = find(m_watches.begin(), m_watches.end(),
        path);
    if (i == m_watches.end()) {
        m_watches.push_back(path);
    }

    STRIGI_MUTEX_UNLOCK (&m_mutex);

    STRIGI_LOG_DEBUG ("strigi.PollingListener.addWatch",
                      "successfully added polling watch for " + path);

    return true;
}
void
PollingListener::rmWatch(const string& path) {
    STRIGI_MUTEX_LOCK (&m_mutex);

    vector<string>::iterator iter = m_watches.begin();
    while (iter != m_watches.end()) {
        if ((*iter).find (path,0) == 0) {
            // directory name begins with param dir --> it's a subfolder of dir
            vector<string>::iterator rmIt = iter;
            iter++;
            m_watches.erase (rmIt);
        } else {
            iter++;
        }
    }

    STRIGI_MUTEX_UNLOCK (&m_mutex);
}

void
PollingListener::addWatches(const set<string>& watches) {
    for (set<string>::iterator iter = watches.begin();
            iter != watches.end(); ++iter) {
        string temp = fixPath(*iter);
        bool match = false;

        STRIGI_MUTEX_LOCK (&m_mutex);
        // try to reduce number of watches
        for (vector<string>::iterator it = m_watches.begin();
                it != m_watches.end(); ++it) {
            if ((temp.length() >= it->length()) && (temp.find(*it) == 0)) {
                // temp starts with *it, it means temp is a subdir of *it
                // we don't add it to watches
                match = true;
                break;
            } else if ((temp.length() < it->length()) && (it->find(temp) == 0)){
                // *it starts with temp, it means *it is a subdir of temp
                // we have to replace *it with temp, begin deleting it
                m_watches.erase(it);
                break;
            }
        }
        STRIGI_MUTEX_UNLOCK (&m_mutex);

        // there's nothing related with temp, add it to watches
        if (!match) {
            addWatch(*iter);
        }
    }
}
void
PollingListener::setIndexedDirectories(const set<string>& dirs) {
    STRIGI_MUTEX_LOCK (&m_mutex);
    m_watches.clear();
    STRIGI_MUTEX_UNLOCK (&m_mutex);

    addWatches(dirs);
}
bool
PollingListener::continueAnalysis() {
    return getState() != Stopping;
}

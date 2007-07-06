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
#ifndef INOTIFYLISTENER_H
#define INOTIFYLISTENER_H

#include "eventlistener.h"
#include "strigi_thread.h"
#include <map>
#include <vector>

class Event;
class PollingListener;

/*!
* @class InotifyListener
* @brief Interacts with kernel inotify monitoring recursively all changes over indexed directories
*/

class InotifyListener : public EventListener
{
    public:
        explicit InotifyListener(std::set<std::string>& indexedDirs);

        ~InotifyListener();

        bool init();

        bool addWatch (const std::string& path);
        void addWatches (const std::set<std::string>& watches,
                         bool enableInterrupt = false);
        void setIndexedDirectories (const std::set<std::string>& dirs);

        void* run(void*);

    protected:
        /*!
        * @param event inotify's event mask
        * returns a string containing the event description
        */
        std::string eventToString(int events);

        /*!
        * @param event the inotify event to analyze
        * returns true if event is to process (ergo is interesting), false otherwise
        */
        bool isEventInteresting (struct inotify_event * event);

        /*!
        * main InotifyListener thread
        */
        void watch ();

        
        void processReindexDirThreadData();
        /*!
        * @param dir removed dir
        * Removes all db entries of files contained into the removed dir.
        * Removes also all inotify watches related to removed dir (including watches over subdirs), there's <b>no need</b> to call rmWatch after invoking that method
        * Updates also m_watches
        */
        void dirRemoved (std::string dir,
                         std::vector<Event*>& events,
                         bool enableInterrupt = false);

        /*!
        * @param dirs removed dirs
        * Removes all db entries of files contained into the removed dirs.
        * Removes also all inotify watches related to removed dirs (including watches over subdirs), there's <b>no need</b> to call rmWatch after invoking that method
        * Optimized for large removal
        * Updates also m_watches
        */
        void dirsRemoved (std::set<std::string> dirs,
                          std::vector<Event*>& events,
                          bool enableInterrupt = false);

        /*!
        * @param wd inotify watch descriptor
        * @param path path associated to inotify watch
        * removes and release an inotify watch. Usually there's no need to use this method.
        * @sa dirRemoved
        */
        void rmWatch(int wd, std::string path);

        void rmWatches(std::map<int, std::string>& watchesToRemove,
                       bool enableInterrupt = false);
        
        void rmWatches(std::set<std::string>& watchesToRemove,
                       bool enableInterrupt = false);
        
        /*!
        * removes and release all inotify watches
        */
        void clearWatches();
        
        void setInterrupt (bool value);
        bool testInterrupt();

        PollingListener* m_pollingListener;
        int m_iInotifyFD;
        int m_iEvents;
        std::map<int, std::string> m_watches; //!< map containing all inotify watches added by InotifyListener. Key is watch descriptor, value is dir path
        bool m_bMonitor;
        bool m_bInterrupt;
        bool m_bInitialized;
        std::map<std::string, time_t> m_toIndex;
        std::set<std::string> m_toWatch;
        std::set<std::string> m_indexedDirs;
        pthread_mutex_t m_watchesLock;
        pthread_mutex_t m_interruptLock;

        class ReindexDirsThread;
        ReindexDirsThread* m_pReindexDirThread;
};

#endif

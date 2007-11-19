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

#include "fslistener.h"
#include "strigi_thread.h"
#include <map>
#include <vector>

class Event;
class PollingListener;

/*!
* @class InotifyListener
* @brief Interacts with kernel inotify monitoring recursively all changes over indexed directories
*/

class InotifyEvent : public FsEvent
{
    public:
        InotifyEvent(int watchID, const std::string& watchName,
                     struct inotify_event* event);

        const std::string description();
        bool regardsDir();

        struct inotify_event* event() { return m_event;}
        char* name();
        
    private:
        struct inotify_event* m_event;
        std::string m_watchName;
        int m_watchID;

};

class PollingListener;

class InotifyListener : public FsListener
{
    public:
        explicit InotifyListener(std::set<std::string>& indexedDirs);

        virtual ~InotifyListener();

        bool init();

    protected:
        /*!
         * @param event the inotify event to analyze
         * returns true if event is to process (ergo is interesting), false otherwise
         */
        bool isEventInteresting (FsEvent * event);

        void stopMonitoring();

        // event methods
        bool pendingEvent();
        FsEvent* retrieveEvent();
        bool isEventValid(FsEvent* event);

        // dir methods
        
        /*!
         * @param dir removed dir
         * Removes all db entries of files contained into the removed dir.
         * Removes also all inotify watches related to removed dir (including watches over subdirs), there's <b>no need</b> to call rmWatch after invoking that method
         * Updates also m_watches
         */
        void dirRemoved (std::string dir, std::vector<Event*>& events);

        // watches methods
        
        bool addWatch (const std::string& path);
        void addWatches (const std::set<std::string>& watches);

        /*!
         * @param wd inotify watch descriptor
         * @param path path associated to inotify watch
         * removes and release an inotify watch. Usually there's no need to use this method.
         * @sa dirRemoved
        */
        void rmWatch(int wd, std::string path);

        void rmWatches(std::map<int, std::string>& watchesToRemove);
        
        void rmWatches(std::set<std::string>& watchesToRemove);
        
        /*!
         * removes and release all inotify watches
         */
        void clearWatches();

    private:
        PollingListener* m_pollingListener;
        int m_iInotifyFD;
        int m_iEvents;
        std::map<int, std::string> m_watches; //!< map containing all inotify watches added by InotifyListener. Key is watch descriptor, value is dir path
};

#endif

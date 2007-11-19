/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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
#ifndef FAMLISTENER_H
#define FAMLISTENER_H

#include "fslistener.h"
#include "strigi_thread.h"

#include <fam.h>
#include <map>
#include <vector>

class Event;

class FamEvent : public FsEvent
{
    public:
        FamEvent(const std::string& watchName, FAMEvent event);

        const std::string description();
        bool regardsDir();

        FAMEvent event() { return m_event;}
        FAMRequest fr() { return m_event.fr; }
        std::string watchName() { return m_watchName;}
        char* name();
        
    private:
        struct FAMEvent m_event;
        std::string m_watchName;
        bool m_regardsDir;
};

/*!
 * @class FamListener
 * @brief Interacts with kernel inotify monitoring recursively all changes over indexed directories
 */
class FamListener : public FsListener
{
    private:
        class Private;
        Private* p;
    
    public:
        explicit FamListener(std::set<std::string>& indexedDirs);

        virtual ~FamListener();

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

        /*!
         * @param dir removed dir
         * Removes all db entries of files contained into the removed dir.
         * Removes also all inotify watches related to removed dir (including watches over subdirs), there's <b>no need</b> to call rmWatch after invoking that method
         * Updates also m_watches
         */
        void dirRemoved (std::string dir, std::vector<Event*>& events);

        // watches methods
        bool addWatch (const std::string& path);

        /*!
         * removes and release all inotify watches
         */
        void clearWatches();
};

#endif

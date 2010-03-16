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
#ifndef FSLISTENER_H
#define FSLISTENER_H

#include "eventlistener.h"
#include "strigi/strigi_thread.h"
#include <map>
#include <vector>

class Event;
class PollingListener;


class FsEvent
{
    public:
        FsEvent (const std::string path, const std::string name);
        virtual ~FsEvent() {};

        enum TYPE {CREATE, UPDATE, DELETE};

        virtual const std::string description() = 0;
        TYPE type() { return m_type; }
        const std::string file() { return m_file;}
        virtual bool regardsDir() = 0;

    protected:
        std::string m_file;
        TYPE m_type;
};


/*!
 * @class FsListener
 * @brief Interacts generic class for low level monitoring facilities
 */

class FsListener : public EventListener
{
    public:
        explicit FsListener(const char* name,
                            std::set<std::string>& indexedDirs);

        virtual ~FsListener();

        virtual bool init() = 0;

        void setIndexedDirectories (const std::set<std::string>& dirs);

        void* run(void*);

    protected:
        void setInitialized () { m_bInitialized = true; }

        /*!
         * @param event the filesystem event to analyze
         * returns true if event is to process (ergo is interesting), false otherwise
         */
        virtual bool isEventInteresting (FsEvent * event) = 0;

        /*!
         * main FsListener thread
         */
        void watch ();

        void bootstrap();
        void reindex();
        bool reindexReq();
        void getReindexDirs(std::set<std::string>&);

        // event methods
        virtual bool pendingEvent() = 0;
        virtual FsEvent* retrieveEvent() = 0;
        virtual bool isEventValid(FsEvent* event) = 0;
        void dumpEvents();

        // dir methods
        
        /*!
         * @param dir removed dir
         * Removes all db entries of files contained into the removed dir.
         * Removes also all watches related to removed dir (including watches over subdirs)
         */
        virtual void dirRemoved (std::string dir,
                                 std::vector<Event*>& events) = 0;

        /*!
         * @param dirs removed dirs
         * @param events all generated events 
         * Removes all db entries of files contained into the removed dirs.
         */
        void dirsRemoved (std::set<std::string> dirs,
                          std::vector<Event*>& events);

        /*!
         * @param dir dir to monitor
         * @param toWatch directories to be watched
         * @param events all generated events
         * Index all files contained inside the directory dir, and all its subdirs
         */
        void recursivelyMonitor (const std::string dir,
                                 std::set<std::string>& toWatch,
                                 std::vector<Event*>& events);
 
        // watches methods
        
        /*!
         * @param watches directories to be watched
         * Add a watch for each directory specified
         */
        void addWatches (const std::set<std::string>& watches);
        virtual bool addWatch (const std::string& path) = 0;
        
        /*!
         * removes and release all watches
         */
        virtual void clearWatches() {};

        bool m_bMonitor;
        bool m_bInitialized;
        bool m_bBootstrapped;
        unsigned int m_counter;

        std::set<std::string> m_indexedDirs;
        std::set<std::string> m_pollingDirs;
        
        bool m_bReindexReq;
        std::set<std::string> m_reindexDirs;
        STRIGI_MUTEX_DEFINE(m_reindexLock);
        std::vector<FsEvent*> m_events;

        PollingListener* m_pollingListener;
};

#endif

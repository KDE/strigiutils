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
#include <map>
#include <vector>

class Event;
class PollingListener;

class InotifyListener : public EventListener
{
    public:
        explicit InotifyListener(std::set<std::string>& indexedDirs);

        ~InotifyListener();
   
        bool init();
       
        bool addWatch (const std::string& path);
        void addWatches (const std::set<std::string>& watches);
        void setIndexedDirectories (const std::set<std::string>& dirs);
       
        void* run(void*);
       
        static bool ignoreFileCallback(const char* path, uint dirlen, uint len, time_t mtime);
        static bool indexFileCallback(const char* path, uint dirlen, uint len, time_t mtime);
        static void watchDirCallback(const char* path, uint len);
       
    private:
        void bootstrap (const std::set<std::string>& dirs);
        std::string eventToString(int events);
        bool isEventInteresting (struct inotify_event * event);
        void watch ();
        void dirRemoved (std::string dir, std::vector<Event*>& events);
        void dirsRemoved (std::set<std::string> dirs, std::vector<Event*>& events);
        void rmWatch(int wd, std::string path);
        void clearWatches();
       
        PollingListener* m_pollingListener;
        int m_iInotifyFD;
        int m_iEvents;
        std::map<unsigned int, std::string> m_watches;
        bool m_bMonitor;
        bool m_bInitialized;
        std::map<std::string, time_t> m_toIndex;
        std::set<std::string> m_toWatch;
        std::set<std::string> m_indexedDirs;
};

#endif

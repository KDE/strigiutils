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
#include "strigi/strigi_thread.h"
#include <map>
#include <vector>

class Event;
class PollingListener;

class InotifyEvent : public FsEvent
{
    public:
        InotifyEvent(int watchID, const std::string& watchName,
                     struct inotify_event* event);

        const std::string description();
        bool regardsDir();

        struct inotify_event* event() { return m_event;}
        char* name();
        int watchID() { return m_watchID;}
        std::string watchName() { return m_watchName;}
        
    private:
        struct inotify_event* m_event;
        std::string m_watchName;
        int m_watchID;

};

/*!
 * @class InotifyListener
 * @brief Uses linux kernel inotify facility in order to keep strigi's index updated. 
 */
class InotifyListener : public FsListener
{
    private:
        class Private;
        Private* p; 
    
    public:
        explicit InotifyListener(std::set<std::string>& indexedDirs);

        virtual ~InotifyListener();

        bool init();

    protected:
        void stopMonitoring();

        // event methods
        bool pendingEvent();
        FsEvent* retrieveEvent();
        bool isEventValid(FsEvent* event);
        bool isEventInteresting (FsEvent * event);

        void dirRemoved (std::string dir, std::vector<Event*>& events);

        // watches methods
        bool addWatch (const std::string& path);

        void clearWatches();
};

#endif

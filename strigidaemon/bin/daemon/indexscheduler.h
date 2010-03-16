/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#ifndef STRIGI_INDEXSCHEDULER_H
#define STRIGI_INDEXSCHEDULER_H

#include <map>
#include <string>
#include <set>
#include <vector>
#include <cstdlib>
#include "strigithread.h"
#include <strigi/diranalyzer.h>
#include <strigi/analyzerconfiguration.h>

class Event;
class EventListenerQueue;

namespace Strigi {
    class IndexManager;
}
#include <iostream>
class StoppableConfiguration : public Strigi::AnalyzerConfiguration {
public:
    bool stop;
    bool indexMore() const {
        return !stop;
    }
    bool addMoreText() const {
        return !stop;
    }
    void setStop(bool s) { stop = s; }
    StoppableConfiguration() :stop(false) {}
};

class IndexScheduler : public StrigiThread, private Strigi::AnalysisCaller {
private:
    std::set<std::string> dirstoindex;
    Strigi::IndexManager* indexmanager;

    EventListenerQueue* m_listenerEventQueue;
    StoppableConfiguration* m_indexerconfiguration;
    void processListenerEvents(std::vector<Event*>& events);

    void* run(void*);
    void index();
public:
    IndexScheduler();
    void addDirToIndex(const std::string& d) {
        dirstoindex.insert(d);
    }
    void setIndexManager(Strigi::IndexManager* m) {
        indexmanager = m;
    }
    void setEventListenerQueue (EventListenerQueue* eventQueue) {
        m_listenerEventQueue = eventQueue;
    }
    void setIndexerConfiguration(StoppableConfiguration* ic) {
        m_indexerconfiguration = ic;
    }
    Strigi::AnalyzerConfiguration& getIndexerConfiguration() const {
        return *m_indexerconfiguration;
    }
    int getQueueSize();
    void startIndexing() { setState(Working); }
    void stopIndexing() {
        setState(Idling);
        m_indexerconfiguration->setStop(true);
    }
    std::string getStateString();
    ~IndexScheduler();
    const std::set<std::string> &getIndexedDirectories() const {
        return dirstoindex;
    }
    void setIndexedDirectories(const std::set<std::string> &d);
    bool continueAnalysis() {
        return getState() == Working;
    }
};

#endif

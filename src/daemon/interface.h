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
#ifndef INTERFACE_H
#define INTERFACE_H

#include "clientinterface.h"

namespace jstreams {
    class IndexManager;
}
class IndexScheduler;
class EventListener;
class FilterManager;

/**
 * This class exposes the daemon functionality to the clients and should be
 * used by the client interfaces. The client interfaces should implement all
 * functions provided here.
 **/

class Interface : public ClientInterface {
private:
    jstreams::IndexManager& manager;
    IndexScheduler& scheduler;
    EventListener* eventListener;
    FilterManager* filterManager;

public:
    Interface(jstreams::IndexManager& m, IndexScheduler& s);
    int countHits(const std::string& query);
    Hits getHits(const std::string& query, int max, int offset);
    std::map<std::string, std::string> getStatus();
    std::string stopDaemon();
    std::string startIndexing();
    std::string stopIndexing();
    std::set<std::string> getIndexedDirectories();
    std::string setIndexedDirectories(std::set<std::string>);
    std::multimap<int, std::string> getFilteringRules();
    void setFilteringRules(const std::multimap<int, std::string>&);
    void setEventListener(EventListener* eListener);
    void setFilterManager(FilterManager* fManager);
};

#endif

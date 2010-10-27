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

#include <strigi/clientinterface.h>

class CombinedIndexManager;
class IndexScheduler;
class EventListener;

/**
 * This class exposes the daemon functionality to the clients and should be
 * used by the client interfaces. The client interfaces should implement all
 * functions provided here.
 **/

class Interface : public ClientInterface {
private:
    CombinedIndexManager& manager;
    IndexScheduler& scheduler;
    EventListener* eventListener;
    bool active;

public:
    Interface(CombinedIndexManager& m, IndexScheduler& s);
    int countHits(const std::string& query);
    Hits getHits(const std::string& query, uint32_t max, uint32_t offset);
    std::map<std::string, std::string> getStatus();
    std::string stopDaemon();
    std::string startIndexing();
    std::string stopIndexing();
    std::set<std::string> getIndexedFiles();
    std::set<std::string> getIndexedDirectories();
    std::string setIndexedDirectories(std::set<std::string>);
    std::multimap<int, std::string> getFilteringRules();
    void setFilteringRules(const std::multimap<int, std::string>&);
    void setFilters(const std::vector<std::pair<bool,std::string> >& rules);
    std::vector<std::pair<bool,std::string> > getFilters();
    void indexFile(const std::string &path, uint64_t mtime,
        const std::vector<char>&);
    std::vector<std::string> getFieldNames();
    std::vector<std::pair<std::string, uint32_t> > getHistogram(const std::string& query, const std::string& field, const std::string& labeltype);

    std::vector<std::string> getBackEnds();
    void setEventListener(EventListener* eListener);
    int countKeywords(const std::string& keywordmatch,
        const std::vector<std::string>& fieldnames);
    std::vector<std::string> getKeywords(const std::string& keywordmatch,
        const std::vector<std::string>& fieldnames,
        uint32_t max, uint32_t offset);

    bool isActive() const { return active; }
};

#endif

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
#ifndef CLIENTINTERFACE_H
#define CLIENTINTERFACE_H

#include "indexeddocument.h"
#include <map>
#include <vector>
#include <set>

class ClientInterface {
protected:
    bool active;
public:
    class Hits {
    public:
        std::vector<jstreams::IndexedDocument> hits;
        std::string error;
    };

    ClientInterface() :active(true) {}
    virtual ~ClientInterface() {}
    virtual int32_t countHits(const std::string& query) = 0;
    virtual Hits getHits(const std::string& query, uint32_t max, uint32_t offset) = 0;
    virtual std::map<std::string, std::string> getStatus() = 0;
    virtual std::string stopDaemon() = 0;
    virtual std::string startIndexing() = 0;
    virtual std::string stopIndexing() = 0;
    virtual std::set<std::string> getIndexedDirectories() = 0;
    virtual std::string setIndexedDirectories(std::set<std::string> d) = 0;
    virtual void setFilters(const std::vector<std::pair<bool,std::string> >& rules) = 0;
    virtual std::vector<std::pair<bool,std::string> > getFilters() = 0;
    virtual std::set<std::string> getIndexedFiles() = 0;
    bool isActive() { return active; }
    virtual std::vector<std::string> getBackEnds() = 0;
    virtual void indexFile(const std::string& path, uint64_t mtime, const std::vector<char>& content) = 0;

    virtual std::vector<std::string> getFieldNames() = 0;
    virtual std::vector<std::pair<std::string, uint32_t> > getHistogram(const std::string& query, const std::string& field, const std::string& labeltype) = 0;
};

#endif

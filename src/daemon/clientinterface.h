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
#include <string>


class ClientInterface {
private:
    ClientInterface* const iface;
public:
    class Hits {
    public:
        std::vector<Strigi::IndexedDocument> hits;
        std::string error;
    };

    ClientInterface(ClientInterface* i) :iface(i) {}
    virtual ~ClientInterface() {}
    virtual int32_t countHits(const std::string& query) {
        return iface->countHits(query);
    }
    virtual Hits getHits(const std::string& query, uint32_t max,
            uint32_t offset) {
        return iface->getHits(query, max, offset);
    }
    virtual std::map<std::string, std::string> getStatus() {
        return iface->getStatus();
    }
    virtual std::string stopDaemon() {
        return iface->stopDaemon();
    }
    virtual std::string startIndexing() {
        return iface->startIndexing();
    }
    virtual std::string stopIndexing() {
        return iface->stopIndexing();
    }
    virtual std::set<std::string> getIndexedDirectories() {
        return iface->getIndexedDirectories();
    }
    virtual std::string setIndexedDirectories(std::set<std::string> d) {
        return iface->setIndexedDirectories(d);
    }
    virtual void setFilters(const std::vector<std::pair<bool,std::string> >& rules) {
        iface->setFilters(rules);
    }
    virtual std::vector<std::pair<bool,std::string> > getFilters() {
        return iface->getFilters();
    }
    virtual std::set<std::string> getIndexedFiles() {
        return iface->getIndexedFiles();
    }
    virtual std::vector<std::string> getBackEnds() {
        return iface->getBackEnds();
    }
    virtual void indexFile(const std::string& path, uint64_t mtime, const std::vector<char>& content) {
        iface->indexFile(path, mtime, content);
    }
    virtual std::vector<std::string> getFieldNames() {
        return iface->getFieldNames();
    }
    virtual std::vector<std::pair<std::string, uint32_t> > getHistogram(
            const std::string& query, const std::string& field,
            const std::string& labeltype) {
        return iface->getHistogram(query, field, labeltype);
    }
    virtual int32_t countKeywords(const std::string& keywordprefix,
            const std::vector<std::string>& fieldnames) {
        return iface->countKeywords(keywordprefix, fieldnames);
    }
    virtual std::vector<std::string> getKeywords(
            const std::string& keywordmatch,
            const std::vector<std::string>& fieldnames, uint32_t max,
            uint32_t offset) {
        return iface->getKeywords(keywordmatch, fieldnames, max, offset);
    }
};

#endif

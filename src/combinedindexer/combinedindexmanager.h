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
#ifndef COMBINEDINDEXMANAGER_H
#define COMBINEDINDEXMANAGER_H

#include "indexmanager.h"
#include <map>
#include <vector>

class CombinedIndexReader;
class CombinedIndexWriter;
class CombinedIndexManager : public jstreams::IndexManager {
private:
    jstreams::IndexManager* writermanager;
    std::vector<jstreams::IndexManager*> readmanagers;
public:
    CombinedIndexManager(const std::string& type, const std::string& dir);
    ~CombinedIndexManager();

    jstreams::IndexReader* getIndexReader();
    jstreams::IndexWriter* getIndexWriter();

    static std::map<std::string, jstreams::IndexManager*(*)(const char*)>
        getFactories();
    static std::vector<std::string> getBackEnds();
};

#endif

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

#include <strigi/indexmanager.h>
#include <map>
#include <vector>
#include <string>

class CombinedIndexReader;
class CombinedIndexManager : public Strigi::IndexManager {
friend class CombinedIndexReader;
private:
    class Private;
    Private* const p;
public:
    CombinedIndexManager(const std::string& type, const std::string& dir);
    ~CombinedIndexManager();

    Strigi::IndexReader* indexReader();
    Strigi::IndexWriter* indexWriter();

    void addReadonlyIndex(const std::string& indexdir, const std::string& type);
    void removeReadonlyIndex(const std::string& indexdir);
};

#endif

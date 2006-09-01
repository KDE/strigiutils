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
#ifndef ESTRAIERINDEXREADER_H
#define ESTRAIERINDEXREADER_H

#include "indexreader.h"
#include <map>
#include <estraier.h>

class EstraierIndexManager;

class EstraierIndexReader : public jstreams::IndexReader {
friend class EstraierIndexManager;
private:
    EstraierIndexManager* manager;
    ESTDB* db;
    EstraierIndexReader(EstraierIndexManager* m, ESTDB* d);
    ~EstraierIndexReader();
    std::string getFragment(ESTDOC* doc, const jstreams::Query& query);
    static ESTCOND* createCondition(const jstreams::Query&);
    static const char* mapId(const std::string& id);
public:
    int32_t countHits(const jstreams::Query&);
    std::vector<jstreams::IndexedDocument> query(const jstreams::Query&);
    std::map<std::string, time_t> getFiles(char depth);
    int countDocuments();
    int32_t countWords();
    int64_t getIndexSize();
    int64_t getDocumentId(const std::string& uri);
    time_t getMTime(int64_t docid);
};

#endif

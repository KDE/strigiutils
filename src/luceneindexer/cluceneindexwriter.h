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
#ifndef CLUCENEINDEXWRITER_H
#define CLUCENEINDEXWRITER_H

#include "indexwriter.h"
#include "indexable.h"
#include "indexerconfiguration.h"

class CLuceneIndexManager;
class CLuceneIndexWriter : public jstreams::IndexWriter {
private:
    CLuceneIndexManager* manager;
    int doccount;

protected:
    void startIndexable(jstreams::Indexable*);
    void finishIndexable(const jstreams::Indexable*);
    void addText(const jstreams::Indexable*, const char* text, int32_t length);
    static void setField(const jstreams::Indexable* idx,
        jstreams::IndexerConfiguration::FieldType type, const TCHAR* name,
        const TCHAR* value);
    static void setField(const jstreams::Indexable* idx,
        jstreams::IndexerConfiguration::FieldType type, const TCHAR* name,
        const std::string& value);
    void setField(const jstreams::Indexable*,
        const std::string& fieldname, const std::string& value);
public:
    explicit CLuceneIndexWriter(CLuceneIndexManager* m);
    ~CLuceneIndexWriter();
    void commit();

    /** cleanup clucene files. this is run when clucene starts up */
    void cleanUp();
    void deleteEntries(const std::vector<std::string>& entries);
    void deleteAllEntries();
    void deleteEntry(const std::string& entry);

    static const TCHAR* mapId(const TCHAR* id);
    static void addMapping(const TCHAR* from, const TCHAR* to);
};

#endif

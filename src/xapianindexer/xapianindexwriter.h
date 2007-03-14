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
#ifndef XAPIANINDEXWRITER_H
#define XAPIANINDEXWRITER_H

#include "indexwriter.h"
#include <xapian.h>

class XapianIndexManager;
class XapianIndexWriter : public Strigi::IndexWriter {
friend class XapianIndexManager;
private:
    XapianIndexManager* manager;
    const std::string indexpath;
    Xapian::WritableDatabase* db;

protected:
    void startIndexable(Strigi::AnalysisResult*);
    void finishIndexable(const Strigi::AnalysisResult*);
    void addText(const Strigi::AnalysisResult* idx, const char* text,
        int32_t length);
    void setField(const Strigi::AnalysisResult* idx, const std::string &fieldname,
        const std::string& value);
    XapianIndexWriter(XapianIndexManager*, Xapian::WritableDatabase*);
    ~XapianIndexWriter();
public:
    void commit();
    void deleteEntries(const std::vector<std::string>& entries);
    void deleteEntry(const std::string& entry);
    int itemsInCache() { return 0; };
};

#endif

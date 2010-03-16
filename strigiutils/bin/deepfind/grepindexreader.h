/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#ifndef GREPINDEXREADER_H
#define GREPINDEXREADER_H

#include <strigi/strigiconfig.h>
#include <strigi/indexreader.h>

#include <map>
#include <time.h>

class GrepIndexReader : public Strigi::IndexReader {
private:
    class Private;
    Private* const p;
public:
    GrepIndexReader(const std::string& dir);
    ~GrepIndexReader();
    int32_t countHits(const Strigi::Query& query);
    std::vector<Strigi::IndexedDocument> query(const Strigi::Query&, int offset,
        int max);
    void getHits(const Strigi::Query&, const std::vector<std::string>& fields,
        const std::vector<Strigi::Variant::Type>& types,
        std::vector<std::vector<Strigi::Variant> >& result, int off, int max);
    std::map<std::string, time_t> files(char depth);
    int32_t countDocuments();
    int32_t countWords();
    int64_t indexSize();
    time_t mTime(const std::string& uri);
    std::vector<std::string> fieldNames();
    std::vector<std::pair<std::string,uint32_t> > histogram(
            const std::string& query, const std::string& fieldname,
            const std::string& labeltype);
    int32_t countKeywords(const std::string& keywordprefix,
        const std::vector<std::string>& fieldnames);
    std::vector<std::string> keywords(
        const std::string& keywordmatch,
        const std::vector<std::string>& fieldnames,
        uint32_t max, uint32_t offset);
};

#endif

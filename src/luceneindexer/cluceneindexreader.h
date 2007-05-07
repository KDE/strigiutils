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
#ifndef CLUCENEINDEXREADER_H
#define CLUCENEINDEXREADER_H

#include "strigiconfig.h"
#include "indexreader.h"

#include <map>
#include <time.h>
namespace lucene {
    namespace index {
        class IndexReader;
    }
}

class CLuceneIndexManager;
class CLUCENEINDEXER_EXPORT CLuceneIndexReader : public Strigi::IndexReader {
friend class CLuceneIndexManager;
friend class CLuceneIndexWriter;
private:
    CLuceneIndexManager* manager;
    class Private;
    int32_t wordcount;
    int32_t doccount;
    const std::string dbdir;
    time_t otime;

    CLuceneIndexReader(CLuceneIndexManager* m, const std::string& dbdir);
    ~CLuceneIndexReader();
    static const TCHAR* mapId(const wchar_t* id);
	static std::wstring mapId(const char* id);
    void openReader();
    void closeReader();
    bool checkReader(bool ensureCurrent = false);

    friend class CLuceneIndexReader::Private;
public:
    int32_t countHits(const Strigi::Query&);
    std::vector<Strigi::IndexedDocument> query(const Strigi::Query&);
    std::map<std::string, time_t> files(char depth);
    int32_t countDocuments();
    int32_t countWords();
    int64_t indexSize();
    int64_t documentId(const std::string& uri);
    time_t mTime(int64_t docid);
    static void addMapping(const TCHAR* from, const TCHAR* to);
    lucene::index::IndexReader* reader;
    std::vector<std::pair<std::string,uint32_t> > histogram(
            const std::string& query, const std::string& fieldname,
            const std::string& labeltype);
    std::vector<std::string> fieldNames();
    int32_t countKeywords(const std::string& keywordprefix,
        const std::vector<std::string>& fieldnames);
    std::vector<std::string> keywords(const std::string& keywordmatch,
        const std::vector<std::string>& fieldnames,
        uint32_t max, uint32_t offset);
};

#endif

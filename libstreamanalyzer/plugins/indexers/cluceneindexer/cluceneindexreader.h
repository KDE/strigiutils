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

#include <strigi/strigiconfig.h>
#include <strigi/indexreader.h>

#include <map>
#include <time.h>
#include <CLucene.h>

class CLuceneIndexManager;
class CLuceneIndexReader : public Strigi::IndexReader {
friend class CLuceneIndexManager;
friend class CLuceneIndexWriter;
private:
    CLuceneIndexManager* manager;
    class Private;
    Private* const p;
    int32_t wordcount;
    int32_t doccount;
    const std::string dbdir;
    struct timeval otime;

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
    std::vector<Strigi::IndexedDocument> query(const Strigi::Query&,
        int off, int max);
    void getHits(const Strigi::Query&, const std::vector<std::string>& fields,
        const std::vector<Strigi::Variant::Type>& types,
        std::vector<std::vector<Strigi::Variant> >& result, int off, int max);
    int32_t countDocuments();
    int32_t countWords();
    int64_t indexSize();
    int64_t documentId(const std::string& uri);
    time_t mTime(int64_t docid);
    time_t mTime(const std::string& uri);
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
    void getChildren(const std::string& parent,
            std::map<std::string, time_t>& );

    // implementation function
    void getDocuments(const std::vector<std::string>& fields,
        const std::vector<Strigi::Variant::Type>& types,
        std::vector<std::vector<Strigi::Variant> >& result, int off, int max);
};

#endif

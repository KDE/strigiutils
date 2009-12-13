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
#ifndef STRIGI_INDEXREADER_H
#define STRIGI_INDEXREADER_H

#include <strigi/strigiconfig.h>
#include "indexeddocument.h"
#include "variant.h"
#include <vector>

namespace Strigi {
class Query;

/**
 * Abstract class that provides read access to a Strigi index.
 *
 * Instances of the class should be obtained by calling the function
 * IndexManager::indexReader() and should not be used from threads other
 * than the thread that called IndexManager::indexReader().
 **/
class STREAMANALYZER_EXPORT IndexReader {
public:
    virtual ~IndexReader() {}
    /**
     * Count the number of documents that reside in this index and that match
     * the given query.
     *
     * @param query the query that will be performed on the index
     * @return the number of documents that match the query
     **/
    virtual int32_t countHits(const Query& query) = 0;
    /**
     * Query the index.
     *
     * @param query the query that will be performed on the index
     * @return the set of documents that match the query and fall in the range
     *         given in the query object
     **/
    virtual std::vector<IndexedDocument> query(const Query&, int off, int max) = 0;

    virtual void getHits(const Strigi::Query& query,
        const std::vector<std::string>& fields,
        const std::vector<Strigi::Variant::Type>& types,
        std::vector<std::vector<Strigi::Variant> >& result,
        int off, int max) = 0;

    /**
     * Obtain the path and mtime of all files in the index that have a parent
     * with the given uri.
     *
     * When listing files in the index it wise to retrieve these files in
     * batches. The function getChildren allows this in such a way that it fits
     * nicely with the scenario of updating the index by traversing directories
     * and container files.
     *
     * All files with the value @p path in the field parent.location will be
     * returned.
     *
     * @param parent The uri of the parent. If it is "" (empty string), all
     *               files without a parent will be retrieved.
     * @param children A map in which the uri and mtime of the children of
     *                 @p parent will be placed.
     **/
    virtual void getChildren(const std::string& /*parent*/,
            std::map<std::string, time_t>& /*children*/) {}
    /**
     * Count the number of documents indexed in the index.
     *
     * @return the number of documents indexed in the index
     **/
    virtual int32_t countDocuments() { return -1; }
    /**
     * Count the number of different words present in the index.
     *
     * @return the number of different words present in the index
     **/
    virtual int32_t countWords() { return -1; }
    /**
     * Determine the size of the index.
     *
     * @return the size of the index
     **/
    virtual int64_t indexSize() { return -1; }
    /**
     * Retrieve the mtime for given document.
     *
     * @param the path of the document
     * @return the mtime the document with the given id. If there is no such
     *         document in the index, 0 is returned.
     **/
    virtual time_t mTime(const std::string& path) = 0;
    /**
     * Retrieve the fieldnames in use in this index.
     **/
    virtual std::vector<std::string> fieldNames() = 0;
    /**
     * Retrieve a histogram of the different values that are in this query.
     **/
    virtual std::vector<std::pair<std::string,uint32_t> > histogram(
            const std::string& query, const std::string& fieldname,
            const std::string& labeltype) = 0;
    /**
     *
     **/
    virtual int32_t countKeywords(const std::string& keywordprefix,
        const std::vector<std::string>& fieldnames) = 0;
    /**
     *
     **/
    virtual std::vector<std::string> keywords(
        const std::string& keywordmatch,
        const std::vector<std::string>& fieldnames,
        uint32_t max, uint32_t offset) = 0;
};

}

#endif

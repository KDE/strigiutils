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
#ifndef INDEXREADER_H
#define INDEXREADER_H

#include "indexeddocument.h"
#include <vector>

namespace jstreams {
class Query;

/**
 * Abstract class that provides read access to a Strigi index.
 *
 * Instances of the class should be obtained by calling the function
 * IndexManager::getIndexReader() and should not be used from threads other
 * than the thread that called IndexManager::getIndexReader().
 **/
class IndexReader {
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
    virtual std::vector<IndexedDocument> query(const Query&) = 0;
    /**
     * Obtain the path and mtime of all files in the index that have a given
     * depth.
     *
     * The usual use case for this function is to retrieve all 'real' files.
     * Files that are indexed directly have depth 0. All files that are
     * are contained within other files have depth > 0.
     *
     * @param depth The depth for the file.
     * @return a vector with path,mtime pairs for all files with a certain
     *         depth
     **/
    virtual std::map<std::string, time_t> getFiles(char depth) = 0;
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
    virtual int64_t getIndexSize() { return -1; }
    /**
     * Obtain the documentid for a given document path.
     *
     * @param the path of a document
     * @return the id of the document or -1 if there is no document with
     *         that id in the index or if there are more than one documents
     *         with that id in the index
     **/
    virtual int64_t getDocumentId(const std::string& uri) = 0;
    /**
     * Retrieve the mtime for given document.
     *
     * @param the id of the document
     * @return the mtime the document with the given id. If there is no such
     *         document in the index, 0 is returned.
     **/
    virtual time_t getMTime(int64_t docid) = 0;
    /**
     * Retrieve the fieldnames in use in this index.
     **/
    virtual std::vector<std::string> getFieldNames() = 0;
    /**
     * Retrieve a histogram of the different values that are in this query.
     **/
    virtual std::vector<std::pair<std::string,uint32_t> > getHistogram(
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
    virtual std::vector<std::string> getKeywords(
        const std::string& keywordmatch,
        const std::vector<std::string>& fieldnames,
        uint32_t max, uint32_t offset) = 0;
};

}

#endif

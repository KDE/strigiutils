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
#ifndef INDEXABLE_H
#define INDEXABLE_H

#include <string>

#include "streamindexer_export.h"

namespace jstreams {

class IndexWriter;
class AnalyzerConfiguration;
class StreamAnalyzer;
class RegisteredField;

template <class T> class StreamBase;
/**
 * Indexed representation of a file.
 *
 * This object allows StreamEndAnalyzer and StreamThroughAnalyer instances to
 * write data associated with a document to the index. The data is
 * automatically written do the index when ~AnalysisResult() is called.
 **/
class STREAMINDEXER_EXPORT AnalysisResult {
friend class IndexWriter;
private:
    int64_t m_id;
    void* m_writerData;
    const time_t m_mtime;
    std::string m_name;
    const std::string m_path;
    std::string m_encoding;
    std::string m_mimetype;
    IndexWriter& m_writer;
    const int m_depth;
    StreamAnalyzer& m_indexer;
    AnalyzerConfiguration& m_indexableconfig;

    /**
     * Create a new AnalysisResult object that will be written to the index.
     *
     * @param path the path of the file
     * @param mt the modification time of the file
     * @param writer the writer with which the indexable will be written upon
     *        destruction
     * @param d the depth at which a document is embedded in other documents.
     *        a depth of 0 means a document is not embedded in another document.
     **/
    AnalysisResult(const std::string& path, const char* name, time_t mt,
        const AnalysisResult& parent);
public:
    AnalysisResult(const std::string& p, time_t mt, IndexWriter& w,
            StreamAnalyzer& indexer);
    /**
     * Write the indexable to the index and release the allocated resources.
     **/
    ~AnalysisResult();
    /**
     * Parse the given stream and index the results into this AnalysisResult object.
     **/
    char index(StreamBase<char>& file);
    /**
     * Index the given stream which represents a child object of this
     * AnalysisResult under the relative name given by @name and versioned with time
     * @mtime.
     **/
    char indexChild(const std::string& name, time_t mt, StreamBase<char>& file);
    /**
     * Associate a fragment of text with the object.
     *
     * @param a pointer to a fragment of utf8 encoded text
     * @param the length of the fragment
     **/
    void addText(const char* text, int32_t length);
    /**
     * Add a name, value pair to the index.
     *
     * @param fieldname utf8 representation of the fieldname, the fieldname
     *
     **/
    void setField(const RegisteredField* fieldname, const std::string& value);
    /**
     * Add a name, value pair to the index.
     *
     * @param fieldname utf8 representation of the fieldname, the fieldname
     *
     **/
    void setField(const RegisteredField* fieldname, uint32_t value);
    /**
     * Get the filename of this Indexble. For the full name see getPath().
     **/
    const std::string& fileName() const;
    /**
     * Retrieve the full name of this AnalysisResult.
     **/
    const std::string& path() const;
    time_t mTime() const;
    char depth() const;
    int64_t id() const;
    void setId(int64_t i);
    void setEncoding(const char* enc);
    const std::string& encoding() const;
    /**
     * Retrieve the IndexWriter specific object associated with this AnalysisResult.
     * This object allows the IndexWriter to store intermediate results
     * associated with this AnalysisResult. IndexWriters using this feature should
     * initialize this value in IndexWriter::startIndexable() and should
     * deallocate the value in the call to IndexWriter::finishIndexable().
     * Each of these functions will be called once during the lifetime of each
     * AnalysisResult.
     **/
    void* writerData() const;
    /**
     * Set the value for the IndexWriter specific data. See
     * IndexWriter::getWriterData() for more details.
     **/
    void setWriterData(void* wd);
    void setMimeType(const std::string& mt);
    const std::string& mimeType() const;
    std::string extension() const;
    AnalyzerConfiguration& config() const;
};

}

#endif

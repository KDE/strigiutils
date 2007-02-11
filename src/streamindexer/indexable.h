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

namespace jstreams {

class IndexWriter;
class IndexerConfiguration;
class StreamIndexer;
class RegisteredField;

template <class T> class StreamBase;
/**
 * Indexed representation of a file.
 *
 * This object allows StreamEndAnalyzer and StreamThroughAnalyer instances to
 * write data associated with a document to the index. The data is
 * automatically written do the index when ~Indexable() is called.
 **/
class Indexable {
friend class IndexWriter;
private:
    int64_t id;
    void* writerData;
    const time_t mtime;
    std::string name;
    const std::string path;
    std::string encoding;
    std::string mimetype;
    IndexWriter& writer;
    const int depth;
    StreamIndexer& indexer;
    IndexerConfiguration& indexableconfig;

    /**
     * Create a new Indexable object that will be written to the index.
     *
     * @param path the path of the file
     * @param mt the modification time of the file
     * @param writer the writer with which the indexable will be written upon
     *        destruction
     * @param d the depth at which a document is embedded in other documents.
     *        a depth of 0 means a document is not embedded in another document.
     **/
    Indexable(const std::string& path, const char* name, time_t mt,
        const Indexable& parent);
public:
    Indexable(const std::string& p, time_t mt, IndexWriter& w,
            StreamIndexer& indexer);
    /**
     * Write the indexable to the index and release the allocated resources.
     **/
    ~Indexable();
    /**
     * Parse the given stream and index the results into this Indexable object.
     **/
    char index(StreamBase<char>& file);
    /**
     * Index the given stream which represents a child object of this
     * Indexable under the relative name given by @name and versioned with time
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
     * Get the filename of this Indexble. For the full name see getPath().
     **/
    const std::string& getFileName() const;
    /**
     * Retrieve the full name of this Indexable.
     **/
    const std::string& getPath() const;
    time_t getMTime() const;
    char getDepth() const;
    int64_t getId() const;
    void setId(int64_t i);
    void setEncoding(const char* enc);
    const std::string& getEncoding() const;
    /**
     * Retrieve the IndexWriter specific object associated with this Indexable.
     * This object allows the IndexWriter to store intermediate results
     * associated with this Indexable. IndexWriters using this feature should
     * initialize this value in IndexWriter::startIndexable() and should
     * deallocate the value in the call to IndexWriter::finishIndexable().
     * Each of these functions will be called once during the lifetime of each
     * Indexable.
     **/
    void* getWriterData() const;
    /**
     * Set the value for the IndexWriter specific data. See
     * IndexWriter::getWriterData() for more details.
     **/
    void setWriterData(void* wd);
    void setMimeType(const std::string& mt);
    const std::string& getMimeType() const;
    std::string getExtension() const;
    IndexerConfiguration& config() const;
};

}

#endif

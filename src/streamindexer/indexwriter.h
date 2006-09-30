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
#ifndef INDEXWRITER_H
#define INDEXWRITER_H

#include <string>
#include <vector>
#include "textutils.h"

namespace jstreams {

template <class T>
class StreamBase;

/*
- create indexwriter
for all streams {
 - create an indexable
 - add the indexwriter to it
 - add a stream to the indexable (optional)
 - add fields to indexable (optional)
 - delete the indexable
}
- delete the indexwriter
*/
class Indexable;
/**
 * Abstract class that provides write access to a Strigi index.
 *
 * Instances of the class should be obtained by calling the function
 * IndexManager::getIndexWriter() and should not be used from threads other
 * than the thread that called IndexManager::getIndexWriter().
 **/
class IndexWriter {
friend class Indexable;
protected:
    virtual void startIndexable(Indexable*) = 0;
    virtual void addText(const Indexable*, const char* text, int32_t length)=0;
    virtual void setField(const Indexable*, const std::string &fieldname,
        const std::string& value) = 0;
//    virtual void setField(const Indexable*, const std::string &fieldname,
//        int64_t value) = 0;
    virtual void finishIndexable(const Indexable*) = 0;
public:
    virtual ~IndexWriter() {}
    /**
     * Flush the accumulated changes to disk.
     **/
    virtual void commit() { return; }
    /**
     * Delete the entries with the given paths from the index.
     *
     * @param entries the paths of the files that should be deleted
     **/
    virtual void deleteEntries(const std::vector<std::string>& entries) = 0;
    /**
     * Delete all indexed documents from the index.
     **/
    virtual void deleteAllEntries() = 0;
    /**
     * Return the number of objects that are currently in the cache.
     **/
    virtual int itemsInCache() { return 0; }
    /**
     * Optimize the index. This can be computationally intensive and can
     * will often cause the index to temporarily use the double amount of
     * diskspace.
     **/
    virtual void optimize() {}
};
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
    const std::string& name;
    std::string encoding;
    std::string mimetype;
    IndexWriter* writer;
    char depth;
public:
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
    Indexable(const std::string& path, time_t mt, IndexWriter* writer, char d)
            :mtime(mt), name(path), writer(writer), depth(d) {
        writer->startIndexable(this);
    }
    /**
     * Write the indexable to the index and release the allocated resources.
     **/
    ~Indexable() { writer->finishIndexable(this); }
    /**
     * Associate a fragment of text with the object.
     *
     * @param a pointer to a fragment of utf8 encoded text
     * @param the length of the fragment
     **/
    void addText(const char* text, int32_t length) {
        writer->addText(this, text, length);
    }
    /**
     * Add a name, value pair to the index.
     *
     * @param fieldname utf8 representation of the fieldname, the fieldname
     *        
     **/
    void setField(const std::string& fieldname, const std::string& value);
    const std::string& getName() const { return name; }
    time_t getMTime() const { return mtime; }
    char getDepth() const { return depth; }
    int64_t getId() const { return id; }
    void setId(int64_t i) { id = i; }
    void setEncoding(const char* enc) { encoding = enc; }
    const std::string& getEncoding() const { return encoding; }
    void* getWriterData() const { return writerData; }
    void setWriterData(void* wd) { writerData = wd; }
    void setMimeType(const std::string& mt) { mimetype = mt; }
    const std::string& getMimeType() const { return mimetype; }
    std::string getExtension() const;
    std::string getFileName() const;
};


}

#endif

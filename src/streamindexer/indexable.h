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

#include "indexwriter.h"
#include "streamindexer.h"
#include <string>

namespace jstreams {

class IndexableConfiguration;
/**
 * Indexed representation of a file.
 *
 * This object allows StreamEndAnalyzer and StreamThroughAnalyer instances to
 * write data associated with a document to the index. The data is
 * automatically written do the index when ~Indexable() is called.
 **/
class StreamIndexer;
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
    Indexable(const std::string& name, time_t mt, const Indexable& parent)
            :mtime(mt), name(name), path(parent.path+'/'+name),
             writer(parent.writer), depth(parent.getDepth()+1),
             indexer(parent.indexer)  {
        writer.startIndexable(this);
    }
public:
    Indexable(const std::string& p, time_t mt, IndexWriter& w,
            StreamIndexer& indexer)
            :mtime(mt), path(p), writer(w), depth(0), indexer(indexer) {
        size_t pos = path.rfind('/');
        if (pos == std::string::npos) {
             name = path;
        } else {
             name = path.substr(pos+1);
        }
        writer.startIndexable(this);
    }
     /**
      * Write the indexable to the index and release the allocated resources.
      **/
    ~Indexable() {
        writer.finishIndexable(this);
    }
    char index(StreamBase<char>& file) {
        return indexer.analyze(*this, &file);
    }
    char indexChild(const std::string& name, time_t mt, StreamBase<char>& file){
        Indexable i(name, mt, *this);
        return indexer.analyze(i, &file);
    }
    /**
     * Associate a fragment of text with the object.
     *
     * @param a pointer to a fragment of utf8 encoded text
     * @param the length of the fragment
     **/
    void addText(const char* text, int32_t length) {
        writer.addText(this, text, length);
    }
    /**
     * Add a name, value pair to the index.
     *
     * @param fieldname utf8 representation of the fieldname, the fieldname
     *        
     **/
    void setField(const std::string& fieldname, const std::string& value);
    const std::string& getFileName() const { return name; }
    const std::string& getPath() const { return path; }
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
};

}

#endif

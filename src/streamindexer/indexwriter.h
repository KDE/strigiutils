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
    virtual void commit() { return; }
    virtual void deleteEntries(const std::vector<std::string>& entries) = 0;
    virtual void deleteAllEntries() = 0;
    virtual int itemsInCache() { return 0; }
    virtual void optimize() {}
};

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
    Indexable(const std::string& n, time_t mt, IndexWriter* w, char d)
            :mtime(mt), name(n), writer(w), depth(d) {
        w->startIndexable(this);
    }
    ~Indexable() { writer->finishIndexable(this); }
    void addText(const char* text, int32_t length) {
        writer->addText(this, text, length);
    }
    void setField(const std::string& fieldname, const std::string& value) {
        if (strchr(fieldname.c_str(), '/') == 0) {
            writer->setField(this, fieldname, value);
        }
    }
    const std::string& getName() const { return name; }
    time_t getMTime() const { return mtime; }
    void setId(int64_t i) { id = i; }
    int64_t getId() const { return id; }
    char getDepth() const { return depth; }
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

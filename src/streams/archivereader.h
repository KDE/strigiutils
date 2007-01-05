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
#ifndef ARCHIVEREADER_H
#define ARCHIVEREADER_H

#include <map>
#include <list>
#include <vector>
#include <string>
#include "substreamprovider.h"

namespace jstreams {

template <class T> class StreamBase;
class SubStreamProvider;
struct EntryInfo;

class DirLister {
private:
    int pos;
    std::vector<jstreams::EntryInfo> entries;
public:
    DirLister(const std::vector<jstreams::EntryInfo>& e)
        : pos(0), entries(e) {
    }
    bool nextEntry(jstreams::EntryInfo& e);
};

/**
 * Abstract class that defines an interface for opening streams and statting
 * files. The class ArchiveReader uses instances of this class to access
 * streams.
 **/
class StreamOpener {
public:
    virtual ~StreamOpener() {}
    virtual jstreams::StreamBase<char>* openStream(const std::string& url) = 0;
    virtual int stat(const std::string& url, jstreams::EntryInfo& e) = 0;
};

class ArchiveReader : public StreamOpener {
private:
    class ArchiveReaderPrivate;
    ArchiveReaderPrivate *p;

    int localStat(const std::string& url, jstreams::EntryInfo& e);
    int32_t maxsize;
public:
    ArchiveReader();
    ~ArchiveReader();
    jstreams::StreamBase<char>* openStream(const std::string& url);
    void closeStream(jstreams::StreamBase<char>*);
    int stat(const std::string& url, jstreams::EntryInfo& e);
    void addStreamOpener(StreamOpener* opener);
    DirLister getDirEntries(const std::string& url);
    bool isArchive(const std::string& url);
    bool canHandle(const std::string& url);
};

class FileStreamOpener : public StreamOpener {
public:
    ~FileStreamOpener() {}
    jstreams::StreamBase<char>* openStream(const std::string& url);
    int stat(const std::string& url, jstreams::EntryInfo& e);
};

} // end namespace jstreams

#endif

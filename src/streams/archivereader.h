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
#include "jstreamsconfig.h"
#include "substreamprovider.h"

/** Namespace for the JStreams Java-style streaming api */
namespace jstreams {

template <class T> class StreamBase;
class SubStreamProvider;
struct EntryInfo;

/**
 * @brief Lists the contents of a directory
 */
class STREAMS_EXPORT DirLister {
private:
    int pos;
    std::vector<jstreams::EntryInfo> entries;
public:
    /**
     * @brief Create a DirLister
     *
     * @param e a vector containing information about each
     * item in the directory
     */
    DirLister(const std::vector<jstreams::EntryInfo>& e)
        : pos(0)
        , entries(e)
    {}
    /**
     * @brief Gets information aobut the next item in the directory
     *
     * @param e an EntryInfo object to populate with information about
     * the next item in the directory
     * @return true if there was another item in the directory, false
     * if there are no further items in the directory
     */
    bool nextEntry(jstreams::EntryInfo& e);
};

/**
 * @brief Abstract class that defines an interface for opening streams and statting
 * files.
 */
class STREAMS_EXPORT StreamOpener {
public:
    /** Default constructor: doesn't do anything */
    virtual ~StreamOpener() {}
    /**
     * @brief Opens a stream for the given url.
     *
     * What is meant by @p url will depend on context.  In the context of an
     * archive with a file system-like directory stucture, it will be the path
     * to the file relative the root of the archive (eg: /foo/bar).
     *
     * @param url unique name of or path to an item
     * @return a stream representing the item given by @p url
     */
    virtual jstreams::StreamBase<char>* openStream(const std::string& url) = 0;
    /**
     * @brief Gets information about a given url
     *
     * What is meant by @p url will depend on context.  In the context of an
     * archive with a file system-like directory stucture, it will be the path
     * to the file relative the root of the archive (eg: /foo/bar).
     *
     * @param url unique name of or path to an item
     * @param e an EntryInfo object to populate with information about
     * the item given by @p url
     * @return 0 if information about the item was found, -1 on error
     */
    virtual int stat(const std::string& url, jstreams::EntryInfo& e) = 0;
};

/**
 * @brief Provides an API for accessing members of an archive.
 */
class STREAMS_EXPORT ArchiveReader : public StreamOpener {
private:
    class ArchiveReaderPrivate;
    ArchiveReaderPrivate *p;

    int localStat(const std::string& url, jstreams::EntryInfo& e);
    int32_t maxsize;
public:
    /** Constructor */
    ArchiveReader();
    /** Destructor */
    ~ArchiveReader();
    /**
     * @brief Opens a stream for a member of the archive.
     *
     * @param url path to a member of the archive, relative to the archive root
     * @return a stream representing the archive member given by @p url
     */
    jstreams::StreamBase<char>* openStream(const std::string& url);
    /**
     * @brief Cleans up after a stream is finished with
     *
     * You should call this when you are done with a stream, so
     * the stream can dealt with properly.
     *
     * @param stream the stream to clean up after
     */
    void closeStream(jstreams::StreamBase<char>* stream);
    /**
     * @brief Gets information about a given url
     *
     * @param url path to a member of the archive, relative to the archive root
     * @param e an EntryInfo object to populate with information about
     * the archive member given by @p url
     * @return 0 if information about the item was found, -1 on error
     */
    int stat(const std::string& url, jstreams::EntryInfo& e);
    /**
     * TODO...
     */
    void addStreamOpener(StreamOpener* opener);
    /**
     * TODO...
     */
    DirLister getDirEntries(const std::string& url);
    /**
     * @brief Check if a file is an archive.
     */
    bool isArchive(const std::string& url);
    /**
     * @brief Check if the archivereader can handle a file.
     */
    bool canHandle(const std::string& url);
};

class STREAMS_EXPORT FileStreamOpener : public StreamOpener {
public:
    ~FileStreamOpener() {}
    /**
     * @brief Opens a stream for the given file
     *
     * @param url the path to the file
     * @return a stream representing the file given by @p url
     */
    jstreams::StreamBase<char>* openStream(const std::string& url);
    /**
     * @brief Gets information about a given file
     *
     * @param url the path to the file
     * @param e an EntryInfo object to populate with information about
     * the file given by @p url
     * @return 0 if information about the file was found, -1 on error
     */
    int stat(const std::string& url, jstreams::EntryInfo& e);
};

} // end namespace jstreams

#endif

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
#ifndef STRIGI_ARCHIVEREADER_H
#define STRIGI_ARCHIVEREADER_H 

#include <string>
#include <strigi/strigiconfig.h>
#include "substreamprovider.h"

namespace Strigi {

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
    virtual InputStream* openStream(const std::string& url) = 0;
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
    virtual int stat(const std::string& url, EntryInfo& e) = 0;
};

/**
 * @brief Provides an API for accessing members of an archive.
 *
 * ArchiveReader doesn't know how to open streams itself.  It must
 * be supplied with StreamOpeners.  For example, a FileStreamOpener
 * can be passed to an ArchiveReader to tell it how to access files
 * on the filesystem.
 */
class STREAMS_EXPORT ArchiveReader : public StreamOpener {
private:
    class ArchiveReaderPrivate;
    ArchiveReaderPrivate* const p;

public:
    /**
     * @brief Lists the contents of a directory
     */
    class STREAMS_EXPORT DirLister {
    public:
        /**
         * @brief Internal helper class.
         */
        class Private;
        /**
         * @brief Create a DirLister
         *
         * @param e a vector containing information about each
         * item in the directory
         */
        DirLister(Private* p);
        /**
         * @brief Copy constructor
         */
        DirLister(const DirLister& dl);
        /**
         * @brief the destructor
         */
        virtual ~DirLister();
        /**
         * @brief assignment operator
         **/
        const DirLister& operator=(const DirLister&);
        /**
         * @brief Gets information about the next item in the directory
         *
         * @param e an EntryInfo object to populate with information about
         * the next item in the directory
         * @return true if there was another item in the directory, false
         * if there are no further items in the directory
         */
        bool nextEntry(EntryInfo& e);
    private:
        Private* const p;
    };

    /** Constructor */
    ArchiveReader();
    /** Destructor */
    ~ArchiveReader();
    /**
     * @brief Opens a stream for a member of an archive.
     *
     * This function is only able to open a stream for an URL
     * if either a registered StreamOpener is capable of doing
     * so (see addStreamOpener()) or the URL can be handled
     * directly (see canHandle()).
     *
     * @param url path to a member of an archive, relative to the archive root
     * @return a stream representing the archive member given by @p url
     */
    InputStream* openStream(const std::string& url);
    /**
     * @brief Cleans up after a stream is finished with
     *
     * Frees the memory used to handle an open stream.
     *
     * Note that this memory will be cleaned up on the destruction
     * of the ArchiveReader anyway.  This just allows you to free
     * up the memory as soon as you are done with a particular stream.
     *
     * @param stream the stream to clean up after
     */
    void closeStream(InputStream* stream);
    /**
     * @brief Gets information about a given url
     *
     * This function is only able to get information about an URL
     * if either a registered StreamOpener is capable of doing
     * so (see addStreamOpener()) or the URL can be handled
     * directly (see canHandle()).
     *
     * @param url path to a member of the archive, relative to the archive root
     * @param e an EntryInfo object to populate with information about
     * the archive member given by @p url
     * @return 0 if information about the item was found, -1 on error
     */
    int stat(const std::string& url, EntryInfo& e);
    /**
     * @brief Adds a StreamOpener for use by the class.
     *
     * ArchiveReader uses StreamOpeners to resolve URLs and get
     * information about and open the resources they point to.
     * For example, a FileStreamOpener may be used to open
     * files on the local system, and another StreamOpener may
     * be capable of accessing files over HTTP.
     *
     * This method provides ArchiveReader with a StreamOpener
     * for opening URLs.  An ArchiveReader object is only
     * capable of handling an URL if there is a StreamOpener
     * capable of either handling the URL or handling part of the
     * URL that points to an archive.
     *
     * @param opener the StreamOpener to add
     */
    void addStreamOpener(StreamOpener* opener);
    /**
     * @brief Lists the contents of the specified directory.
     *
     * If the @p url is one that the ArchiveReader can handle
     * (see canHandle()), and refers to a directory (or an
     * archive), it will fetch a list of the contents of the
     * directory.
     *
     * @param url the directory to list the contents of
     * @return a DirLister containing information about the
     * contents of the directory, or an empty DirLister if the
     * file did not exist, was not a directory or could not be
     * handled
     */
    DirLister dirEntries(const std::string& url);
    /**
     * @brief Check if a file is an archive.
     *
     * Checks the StreamOpeners, in the order they were passed to
     * addStreamOpener(), to find the file specified by @p url.
     * It considers an archive to be a file that is also a directory
     * (from the point of view of the StreamOpener).
     *
     * It will only check the first StreamOpener that recognises
     * the given URL.
     *
     * @param url URL of the file to check, as recognised by a
     * StreamOpener registered with addStreamOpener()
     * @return true if the URL specifies an archive, false if
     * the URL was not recognised by any StreamOpener, or the URL
     * does not specify an archive
     */
    bool isArchive(const std::string& url);
    /**
     * @brief Check if the archivereader can handle a file.
     *
     * Checks to see if the object at the location described by
     * @p url exists and is an archive, or if any parent of it
     * is an archive.
     *
     * A parent is determined by examining the URL and splitting
     * it into a path delimited by @c /.
     *
     * @param url the URL of the object to check for
     * @return true if the object described by @p url or any parent
     * of it is an archive, false if @p url and all its parents
     * are not archives
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
    InputStream* openStream(const std::string& url);
    /**
     * @brief Gets information about a given file
     *
     * @param url the path to the file
     * @param e an EntryInfo object to populate with information about
     * the file given by @p url
     * @return 0 if information about the file was found, -1 on error
     */
    int stat(const std::string& url, EntryInfo& e);
};

} // end namespace Strigi

#endif

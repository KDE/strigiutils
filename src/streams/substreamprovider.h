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
#ifndef SUBSTREAMPROVIDER
#define SUBSTREAMPROVIDER

#include <string>
#include "jstreamsconfig.h"
#include "streambase.h"

/** Namespace for the JStreams Java-style streaming api */
namespace jstreams {

/**
 * @brief Provides information about the current substream
 */
struct EntryInfo {
    /**
     * @brief Name of the substream
     *
     * This will be a file name in almost all cases
     */
    std::string filename;
    /** The size of the substream */
    int32_t size;
    /** The last modification time of the substream */
    unsigned mtime;
    /** Represents the type of the substream */
    enum Type {
        Unknown=0 /**< Indicates that the type of the substream is unknown */,
        Dir=1 /**< Indicates that the substream represents a directory */,
        File=2 /**< Indicates that the substream represents a file */
    };
    /** The type of the substream */
    Type type;
    /** Initialises the elements to sane defaults */
    EntryInfo() :size(-1), mtime(0), type(Unknown) {}
};

/**
 * @brief Extracts substreams from an InputStream
 *
 * This class extract substreams, such as files and directories,
 * from an InputStream.  For example, it may extract files
 * and directories from a tar achive, or a uncompressed data from
 * a gzipped data stream.
 */
class STREAMS_EXPORT SubStreamProvider {
protected:
    /** The status of the SubStreamProvider */
    StreamStatus status;
    /**
     * @brief String representation of the last error, or
     * an empty string otherwise
     */
    std::string error;
    /** The input stream to extract the substreams from */
    StreamBase<char> *input;
    /** The current substream */
    StreamBase<char> *entrystream;
    /** Information about the current substream */
    EntryInfo entryinfo;
public:
    /**
     * @brief Initialize the SubStreamProvider
     *
     * This just sets the members to sane defaults, and assigns
     * @p i to @p input
     *
     * @param i the inputstream that will be split into substreams
     */
    SubStreamProvider(StreamBase<char> *i)
        : status(Ok)
        , input(i)
        , entrystream(0)
    {}
    /**
     * @brief Releases the memory used by entrystream, if there is one
     *
     * NB: does not delete input
     */
    virtual ~SubStreamProvider()
    {
        if (entrystream) {
            delete entrystream;
        }
    }
    /**
     * @brief  Return the status of the stream.
     **/
    StreamStatus getStatus() const
    {
        return status;
    }
    /**
     * Get the next substream
     *
     * This function should return a stream representing the next
     * substream that can be read from the input stream, or 0 if
     * there are no more substreams.
     *
     * For example, if the input stream were a tar archive, it would
     * return a stream representing the next file or directory in the
     * archive.
     *
     * It should also load the substream (the same one that was returned)
     * into entrystream, and set entryinfo to an EntryInfo giving
     * information about the substream.
     *
     * If there may be more substreams, but an error occurred, 0 should
     * be returned, the status should be set to Error and an error message
     * should be set.
     *
     * @return the next substream, or 0 if there are no more substreams
     * or an error occurred
     */
    virtual StreamBase<char>* nextEntry() = 0;
    /**
     * @brief get the current substream
     */
    StreamBase<char>* currentEntry()
    {
        return entrystream;
    }
    /**
     * @brief get information about the current substream
     */
    const EntryInfo &getEntryInfo() const
    {
        return entryinfo;
    }
    /**
     * @brief  Return a string representation of the last error.
     * If no error has occurred, an empty string is returned.
     **/
    const char* getError() const
    {
        return error.c_str();
    }
};

} // end namespace jstreams

#endif

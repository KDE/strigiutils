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
#ifndef STRIGI_SUBSTREAMPROVIDER
#define STRIGI_SUBSTREAMPROVIDER

#include <string>
#include <map>
#include "streambase.h"

namespace Strigi {

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
    /**
     * @brief Extendable property map
     **/
    std::map<std::string, std::string> properties;
    /** The size of the substream */
    int64_t size;
    /** The last modification time of the substream */
    time_t mtime;
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
 * This class extracts substreams, such as files and directories,
 * from an InputStream.  For example, it may extract files
 * and directories from a tar archive, or uncompressed data from
 * a gzipped data stream.
 */
class STREAMS_EXPORT SubStreamProvider {
protected:
    /** The status of the SubStreamProvider */
    StreamStatus m_status;
    /**
     * @brief String representation of the last error, or
     * an empty string otherwise
     */
    std::string m_error;
    /** The input stream to extract the substreams from */
    InputStream *m_input;
    /** The current substream */
    InputStream *m_entrystream;
    /** Information about the current substream */
    EntryInfo m_entryinfo;
public:
    /**
     * @brief Initialize the SubStreamProvider
     *
     * This just sets the members to sane defaults, and assigns
     * @p input to @p m_input
     *
     * @param input the inputstream that will be split into substreams
     */
    SubStreamProvider(InputStream *input)
        : m_status(Ok)
        , m_input(input)
        , m_entrystream(0)
    {}
    /**
     * @brief Releases the memory used by entrystream, if there is one
     *
     * NB: does not delete input
     */
    virtual ~SubStreamProvider()
    {
        if (m_entrystream) {
            delete m_entrystream;
        }
    }
    /**
     * @brief  Return the status of the stream.
     **/
    StreamStatus status() const
    {
        return m_status;
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
    virtual InputStream* nextEntry() = 0;
    /**
     * @brief get the current substream
     */
    InputStream* currentEntry()
    {
        return m_entrystream;
    }
    /**
     * @brief get information about the current substream
     */
    const EntryInfo &entryInfo() const
    {
        return m_entryinfo;
    }
    /**
     * @brief  Return a string representation of the last error.
     * If no error has occurred, an empty string is returned.
     **/
    const char* error() const
    {
        return m_error.c_str();
    }
};

} // end namespace Strigi

#endif

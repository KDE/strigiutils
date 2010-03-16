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
#ifndef STRIGI_STRINGTERMINATEDSUBSTREAM
#define STRIGI_STRINGTERMINATEDSUBSTREAM

#include <strigi/strigiconfig.h>
#include "streambase.h"

namespace Strigi {

/**
 * @brief Reads a stream up to a given terminator.
 *
 * This class stops reading data when either the end of the
 * underlying stream is reacher, or a given terminator is encountered
 * in the the stream.
 */
class STREAMS_EXPORT StringTerminatedSubStream : public InputStream {
private:
    class Private;
    Private* const p;
public:
    /**
     * @brief Create a stream from an InputStream that stops when it reaches
     * the given terminator.
     *
     * @param i the underlying InputStream to read the data from
     * @param terminator the terminator indicating the end of this substream
     */
    StringTerminatedSubStream(InputStream* i, const std::string& terminator);
    ~StringTerminatedSubStream();
    int32_t read(const char*& start, int32_t min=0, int32_t max=0);
    int64_t reset(int64_t pos);
    /**
     * @brief Finds the offset in the underlying substream.
     *
     * If the underlying InputStream had already been read from before
     * it was passed to the constructor of this class, it will have a
     * different position to this InputStream.  This function returns
     * the different between the start of the underlying stream and
     * the start of this stream.
     *
     * To get the current position in the underlying substream, you should
     * do
     * @code
     * int64_t pos = stream.offset() + stream.position();
     * @endcode
     *
     * @return the different between the start of the underlying stream
     * and the start of this stream
     */
    int64_t offset() const;
};

} //end namespace Strigi

#endif

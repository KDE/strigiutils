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
#ifndef STRIGI_SUBINPUTSTREAM_H
#define STRIGI_SUBINPUTSTREAM_H

#include <strigi/strigiconfig.h>
#include "streambase.h"

namespace Strigi {

/**
 * @brief Provides access to part of an InputStream only.
 *
 * This class allows you to treat a part of in InputStream as
 * a stream in its own right.  For example, <tt>substream.position()</tt>
 * will return a value relative to the point that the underlying
 * stream was at when the SubInputStream was created.
 *
 * It will prevent data being read from before the point at
 * which the underlying stream was when the substream was
 * created.  Optionally, it can limit the size of the sub
 * stream, preventing data from being read past a certain
 * point even if the underlying stream has more data.
 */
class STREAMS_EXPORT SubInputStream : public InputStream {
private:
    const int64_t m_offset;
    InputStream *m_input;
public:
    /**
     * @brief Create a SubInputStream from an InputStream.
     *
     * If the @p size parameter is not given, or is -1,
     * there is no limit on the amount of data that can
     * be read from the stream other than the amount of
     * data remaining in the underlying stream.
     *
     * If @p size is less than -1, behaviour is undefined.
     *
     * @param input the stream to be used as the source of the data
     * for this substream
     * @param size the maximum amount of data that may be read
     * from this substream
     */
    explicit SubInputStream(InputStream *input, int64_t size=-1);
    int32_t read(const char*& start, int32_t min, int32_t max);
    int64_t reset(int64_t pos);
    int64_t skip(int64_t ntoskip);
};

} //end namespace Strigi

#endif

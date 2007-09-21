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
#ifndef STRIGI_FILEINPUTSTREAM_H
#define STRIGI_FILEINPUTSTREAM_H

#include "bufferedstream.h"

namespace Strigi {

/**
 * @brief Provides buffered access to a file
 */
class STREAMS_EXPORT FileInputStream : public BufferedInputStream {
private:
    FILE *file;
    std::string filepath;

    int32_t fillBuffer(char* start, int32_t space);
    /** The default buffer size, only used as a default argument to the constructor */
    static const int32_t defaultBufferSize;
protected:
    void open(FILE* file, const char* filepath,
        int32_t buffersize=defaultBufferSize);

public:
    /**
     * @brief Create an InputStream to access a file
     *
     * @param filepath the name of the file to open
     * @param buffersize the size of the buffer to use
     */
    explicit FileInputStream(const char* filepath,
        int32_t buffersize=defaultBufferSize);
    FileInputStream(FILE* file, const char* filepath,
        int32_t buffersize=defaultBufferSize);
    ~FileInputStream();
};

} // end namespace Strigi

#endif


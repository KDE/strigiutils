/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2009 Jos van den Oever <jos@vandenoever.info>
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
#ifndef STRIGI_MMAPFILEINPUTSTREAM_H
#define STRIGI_MMAPFILEINPUTSTREAM_H

#include <strigi/streambase.h>

namespace Strigi {

/**
 * @brief Provides buffered access to a file
 */
class MMapFileInputStream : public InputStream {
private:
    const char *buffer;

    void open(FILE* f, const char* path);

    int32_t read(const char*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t reset(int64_t pos);
public:
    /**
     * @brief Create an InputStream to access a file
     *
     * @param filepath the name of the file to open
     */
    explicit MMapFileInputStream(const char* filepath);
    ~MMapFileInputStream();
};

} // end namespace Strigi

#endif


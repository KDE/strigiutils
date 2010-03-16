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
#ifndef STRIGI_FILEREADER_H
#define STRIGI_FILEREADER_H

#include "streambase.h"

namespace Strigi {

class FileInputStream;
class InputStreamReader;

/**
 * @brief Reads and decodes a text file into a Unicode stream
 */
class FileReader : public StreamBase<wchar_t> {
    FileInputStream* input;
    InputStreamReader* reader;
public:
    /**
     * @brief Create a FileReader for a text file
     *
     * @param fname the name of the text file to decode
     * @param encoding_scheme the encoding of the file
     * @param cachelen the buffer size to use when reading the file
     * @param cachebuff currently unused
     */
    explicit FileReader(const char* fname, const char* encoding_scheme=NULL,
        const int32_t cachelen = 13,
        const int32_t cachebuff = 14 );
    /** Destructor */
    ~FileReader();
    int32_t read(const wchar_t*& start, int32_t min, int32_t max);
    int64_t reset(int64_t pos);
};

} // end namespace Strigi

#endif

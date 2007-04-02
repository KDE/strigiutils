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
#include "strigiconfig.h"
#include "filereader.h"
#include "fileinputstream.h"
#include "inputstreamreader.h"

using namespace Strigi;

FileReader::FileReader(const char* fname, const char* encoding_scheme,
        int32_t cachelen, int32_t /*cachebuff*/) {
    input = new FileInputStream(fname, cachelen);
    reader = new InputStreamReader(input, encoding_scheme);
}
FileReader::~FileReader() {
    if (reader) delete reader;
    if (input) delete input;
}
int32_t
FileReader::read(const wchar_t*& start, int32_t min, int32_t max) {
    int32_t nread = reader->read(start, min, max);
    if (nread < -1) {
        m_error = reader->error();
        m_status = Error;
        return nread;
    } else if (nread == -1) {
        m_status = Eof;
    }
    return nread;
}
int64_t
FileReader::reset(int64_t newpos) {
    m_position = reader->reset(newpos);
    if (m_position < -1) {
        m_status = Error;
        m_error = reader->error();
    }
    return m_position;
}


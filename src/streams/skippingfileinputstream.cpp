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
#include "skippingfileinputstream.h"
#include <config.h>
#include <strigi/strigiconfig.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdlib>

using namespace Strigi;
using namespace std;

SkippingFileInputStream::SkippingFileInputStream(const char* filepath) :p(0) {
    buffer = 0;
    buffersize = 0;
    if (filepath == 0) {
        file = 0;
        m_error = "No filename was provided.";
        m_status = Error;
        return;
    }
    FILE* f = fopen(filepath, "rb");
    open(f, filepath);
}
void
SkippingFileInputStream::open(FILE* f, const char* path) {
    // try to open the file for reading
    file = f;
    filepath.assign(path);
    if (file == 0) {
        // handle error
        m_error = "Could not read file '";
        m_error += filepath;
        m_error += "': ";
        m_error += strerror(errno);
        m_status = Error;
        return;
    }
    // determine file size. if the stream is not seekable, the size will be -1
    if (fseeko(file, 0, SEEK_END) == -1) {
        m_size = -1;
    } else {
        m_size = ftello(file);
        fseeko(file, 0, SEEK_SET);
        // if the file has size 0, make sure that it's really empty
        // this is useful for filesystems like /proc that report files as size 0
        // for files that do contain content
        if (m_size == 0) {
            char dummy[1];
            size_t n = fread(dummy, 1, 1, file);
            if (n == 1) {
                m_size = -1;
                fseeko(file, 0, SEEK_SET);
            } else {
                fclose(file);
                file = 0;
                return;
            }
        }
    }
}
SkippingFileInputStream::~SkippingFileInputStream() {
    if (file) {
        if (fclose(file)) {
            // handle error
            m_error = "Could not close file '" + filepath + "'.";
        }
    }
    free(buffer);
}
int32_t
SkippingFileInputStream::read(const char*& start, int32_t _min, int32_t _max) {
    int32_t n = max(_min, _max);
    if (n > buffersize) {
        buffer = (char*)realloc(buffer, n);
    }
    int32_t nr = fread(buffer, 1, n, file);
    m_position = ftell(file);
    if (nr != n) {
        if (ferror(file)) {
            m_status = Error;
        } else {
            m_status = Eof;
            if (m_size == -1) {
                m_size = m_position;
            }
        }
    }
    start = buffer;
    return nr;
}
int64_t
SkippingFileInputStream::skip(int64_t ntoskip) {
    int64_t oldpos = m_position;
    if (reset(m_position + ntoskip) == -2) return -2;
    return m_position - oldpos;
}
int64_t
SkippingFileInputStream::reset(int64_t pos) {
    if (m_size >= 0 && pos > m_size) pos = m_size;
    if (fseek(file, pos, SEEK_SET)) {
        m_status = Error;
        return -2;
    }
    m_position = ftell(file);
    m_status = (m_position == m_size) ?Eof :Ok;
    return m_position;
}

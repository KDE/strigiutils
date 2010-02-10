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
#include "mmapfileinputstream.h"
#include <config.h>
#include <strigi/strigiconfig.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <algorithm>

using namespace Strigi;
using namespace std;

MMapFileInputStream::MMapFileInputStream(const char* filepath) {
    int fd = ::open(filepath, O_RDONLY);
    struct stat sb;
    if (fd == -1 || fstat(fd, &sb) == -1) {
        // handle error
        buffer = 0;
        m_error = "Could not read file '";
        m_error += filepath;
        m_error += "': ";
        m_error += strerror(errno);
        m_status = Error;
        close(fd);
        return;
    }
    m_size = sb.st_size;
    if (m_size > 0) {
        buffer = (const char*)mmap(NULL, m_size, PROT_READ, MAP_PRIVATE, fd, 0);
    } else {
        buffer = 0;
    }
    close(fd);
    if (buffer == (const char*)MAP_FAILED) {
        // handle error
        buffer = 0;
        m_error = "Could not read file '";
        m_error += filepath;
        m_error += "': ";
        m_error += strerror(errno);
        m_status = Error;
        return;
    }
}
MMapFileInputStream::~MMapFileInputStream() {
    if (buffer) {
        munmap((void*)buffer, m_size);
    }
}
int32_t
MMapFileInputStream::read(const char*& start, int32_t _min, int32_t _max) {
    if (m_status == Error) return -2;
    if (m_status == Eof) return -1;
    int32_t n = max(_min, _max);
    if (n >= m_size - m_position) {
        m_status = Eof;
        n = (int32_t)(m_size - m_position);
    }
    start = buffer + m_position;
    m_position += n;
    return n;
}
int64_t
MMapFileInputStream::skip(int64_t ntoskip) {
    if (ntoskip >= m_size - m_position) {
        ntoskip = m_size - m_position;
        m_status = Eof;
    }
    m_position += ntoskip;
    return ntoskip;
}
int64_t
MMapFileInputStream::reset(int64_t pos) {
    if (m_status == Error) return -2;
    if (pos >= m_size) {
        m_status = Eof;
        m_position = m_size;
    } else if (pos < 0) {
        m_position = 0;
        m_status = Ok;
    } else {
        m_position = pos;
        m_status = Ok;
    }
    return m_position;
}

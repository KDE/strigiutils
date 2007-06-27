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
#include "subinputstream.h"
#include "strigiconfig.h"
#include <iostream>
#include <cassert>

using namespace std;
using namespace Strigi;

SubInputStream::SubInputStream(InputStream *i, int64_t length)
        : m_offset(i->position()), m_input(i) {
    assert(length >= -1);
//    printf("substream m_offset: %lli\n", m_offset);
    m_size = length;
}
int32_t
SubInputStream::read(const char*& start, int32_t min, int32_t max) {
    if (m_size != -1) {
        const int64_t left = m_size - m_position;
        if (left == 0) {
            return -1;
        }
        // restrict the amount of data that can be read
        if (max <= 0 || max > left) {
            max = (int32_t)left;
        }
        if (min > max) min = max;
        if (left < min) min = (int32_t)left;
    }
    int32_t nread = m_input->read(start, min, max);
    if (nread < -1) {
        fprintf(stderr, "substream too short.\n");
        m_status = Error;
        m_error = m_input->error();
    } else if (nread < min) {
        if (m_size == -1) {
            m_status = Eof;
            if (nread > 0) {
                m_position += nread;
                m_size = m_position;
            }
        } else {
//            fprintf(stderr, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! nread %i min %i max %i m_size %lli\n", nread, min, max, m_size);
//            fprintf(stderr, "pos %lli parentpos %lli\n", m_position, m_input->position());
//            fprintf(stderr, "status: %i error: %s\n", m_input->status(), m_input->error());
            // we expected data but didn't get enough so that's an error
            m_status = Error;
            m_error = "Premature end of stream\n";
            nread = -2;
        }
    } else {
        m_position += nread;
        if (m_position == m_size) {
            m_status = Eof;
        }
    }
    return nread;
}
int64_t
SubInputStream::reset(int64_t newpos) {
    assert(newpos >= 0);
//    fprintf(stderr, "subreset pos: %lli newpos: %lli m_offset: %lli\n", m_position,
//        newpos, m_offset);
    m_position = m_input->reset(newpos + m_offset);
    if (m_position < m_offset) {
        cerr << "########### m_position " << m_position << " newpos " << newpos
            << endl;
        m_status = Error;
        m_error = m_input->error();
    } else {
        m_position -= m_offset;
        m_status = m_input->status();
    }
    return m_position;
}
int64_t
SubInputStream::skip(int64_t ntoskip) {
//    printf("subskip pos: %lli ntoskip: %lli m_offset: %lli\n", m_position, ntoskip, m_offset);
    if (m_size == m_position) {
        m_status = Eof;
        return -1;
    }
    if (ntoskip == 0) return 0;
    if (m_size != -1) {
        const int64_t left = m_size - m_position;
        // restrict the amount of data that can be skipped
        if (ntoskip > left) {
            ntoskip = left;
        }
    }
    int64_t skipped = m_input->skip(ntoskip);
    if (m_input->status() == Error) {
        m_status = Error;
        m_error = m_input->error();
    } else {
        m_position += skipped;
        if (m_position == m_size) {
            m_status = Eof;
        } else if (skipped <= 0) {
            m_status = Error;
            m_error = "Premature end of stream\n";
            skipped = -2;
        }
    }
    return skipped;
}

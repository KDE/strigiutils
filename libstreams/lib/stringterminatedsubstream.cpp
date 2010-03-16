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
#include <strigi/stringterminatedsubstream.h>
#include <strigi/strigiconfig.h>
#include <strigi/kmpsearcher.h>
#include <cassert>
#include <iostream>

using namespace std;
using namespace Strigi;

// TODO add a mechanism that avoid searching for a stop point again after a
// reset

class StringTerminatedSubStream::Private {
public:
    KmpSearcher m_searcher;
    const int64_t m_offset;
    int64_t furthest;
    InputStream* m_input;

    Private(InputStream* i, const std::string& terminator)
            : m_offset(i->position()), furthest(0), m_input(i) {
        m_searcher.setQuery(terminator);
    }
};

StringTerminatedSubStream::StringTerminatedSubStream(InputStream* i,
        const std::string& terminator) :p(new Private(i, terminator)) {
}
StringTerminatedSubStream::~StringTerminatedSubStream() {
    delete p;
}
int64_t
StringTerminatedSubStream::offset() const {
    return p->m_offset;
}
int32_t
StringTerminatedSubStream::read(const char*& start, int32_t min, int32_t max) {
    if (m_status == Eof) return -1;
    if (m_status == Error) return -2;
    int32_t nread;

    // check if we already read enough
    const int64_t pos = p->m_input->position();
    nread = (int32_t)(p->furthest - pos);
    if (min <= nread) {
        if (max <= 0 || max > nread) {
            max = nread;
        }
        nread = p->m_input->read(start, min, max);
        assert(nread >= -1);
        if (nread > 0) {
            m_position += nread;
            if (m_position == m_size) {
                m_status = Eof;
            }
        }
        return nread;
    }

    // convenience parameter
    int32_t tl = p->m_searcher.queryLength();

    // increase min and max to accommodate for the length of the terminator
    int32_t tlmin = min;
    int32_t tlmax = max;
    if (tlmin == 0) {
        tlmin = 1 + tl;
    } else {
        tlmin += tl;
    }
    if (tlmax > 0 && tlmax < tlmin) tlmax = tlmin;

    nread = p->m_input->read(start, tlmin, tlmax);
    if (nread == -1) {
        m_status = Eof;
        return nread;
    }
    if (nread < -1) {
        m_status = Error;
        m_error = p->m_input->error();
        return nread;
    }

    const char* end = p->m_searcher.search(start, nread);
    if (end) {
        // the end signature was found
        nread = (int32_t)(end - start);
        // signal the end of stream at the next call
        m_status = Eof;
        // set input stream to point after the terminator
        p->m_input->reset(pos + nread + tl);
    } else if (nread >= tlmin) {
        // we are not at or near the end and read the required amount
        // reserve the last bit of buffer for rereading to match the terminator
        // in the next call
        nread -= tl;
        p->furthest = pos + nread;
        // we rewind, but the pointer 'start' will stay valid nontheless
        p->m_input->reset(pos + nread);
    } else if (max != 0 && nread > max) {
        // we are near the end of the stream but cannot pass all data
        // at once because the amount read is larger than the amount to pass
        p->furthest = pos + nread;
        p->m_input->reset(pos + max);
        nread = max;
    } else {
        // we are at the end of the stream, so no need to rewind
        // signal the end of stream at the next call
        m_status = Eof;
    }
    if (nread > 0) m_position += nread;
    if (m_status == Eof) {
        m_size = m_position;
    }
    return nread;
}
int64_t
StringTerminatedSubStream::reset(int64_t newpos) {
    m_position = p->m_input->reset(newpos + p->m_offset);
    if (m_position >= p->m_offset) {
        m_position -= p->m_offset;
        if (m_position != m_size) m_status = Ok;
    } else {
        // the stream is not positioned at a valid m_position
        m_status = Error;
        m_position = -1;
    }
    return m_position;
}

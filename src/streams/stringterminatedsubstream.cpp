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
#include "stringterminatedsubstream.h"

using namespace Strigi;

// TODO add a mechanism that avoid searching for a stop point again after a
// reset

int32_t
StringTerminatedSubStream::read(const char*& start, int32_t min, int32_t max) {
//    fprintf(stderr, "StringTerminatedSubStream::read %i\n", m_status);
    if (m_status == Eof) return -1;
    if (m_status == Error) return -2;

    // convenience parameter
    int32_t tl = m_searcher.queryLength();

    // increase min and max to accommodate for the length of the terminator
    int32_t tlmin = min;
    int32_t tlmax = max;
    if (tlmin == 0) {
        tlmin = 1 + tl;
    } else {
        tlmin += tl;
    }
    if (tlmax > 0 && tlmax < tlmin) tlmax = tlmin;

    int64_t pos = m_input->position();
//    fprintf(stderr, "STSS tl: %i tlmin %i tlmax %i %lli pos\n", tl, tlmin, tlmax, pos);
    int32_t nread = m_input->read(start, tlmin, tlmax);
//    fprintf(stderr, "stss str %i pos %lli m_offset %lli\n", nread, pos, m_offset);
    if (nread == -1) {
        m_status = Eof;
        return nread;
    }
    if (nread < -1) {
        m_status = Error;
        m_error = m_input->error();
        return nread;
    }

    const char* end = m_searcher.search(start, nread);
    if (end) {
        // the end signature was found
        //printf("THE END %p %p %s\n", start, end, m_searcher.query().c_str());
        //printf("TE %i '%.*s'\n", end-start, 10, end);
        nread = end - start;
        // signal the end of stream at the next call
        m_status = Eof;
        // set input stream to point after the terminator
        m_input->reset(pos + nread + tl);
    } else if (nread >= tlmin) {
        // we are not at or near the end and read the required amount
        // reserve the last bit of buffer for rereading to match the terminator
        // in the next call
        nread -= tl;
        // we rewind, but the pointer 'start' will stay valid nontheless
        m_input->reset(pos + nread);
    } else if (max != 0 && nread > max) {
        // we are near the end of the stream but cannot pass all data
        // at once because the amount read is larger than the amount to pass
        m_input->reset(pos + max);
        nread = max;
    } else {
        // we are at the end of the stream, so no need to rewind
        // signal the end of stream at the next call
        m_status = Eof;
    }
//    fprintf(stderr, "stss: %i '%.*s'\n", nread, (20>nread)?nread:20, start);
    if (nread > 0) m_position += nread;
    if (m_status == Eof) {
//        fprintf(stderr, "Eof size %lli pos %lli\n", m_size, m_position);
        m_size = m_position;
    }
//    fprintf(stderr, "str2 %i %i\n", nread, m_status);
    return nread;
}
int64_t
StringTerminatedSubStream::reset(int64_t newpos) {
    m_position = m_input->reset(newpos+m_offset);
    if (m_position >= m_offset) {
        m_position -= m_offset;
        if (m_position != m_size) m_status = Ok;
    } else {
        // the stream is not positioned at a valid m_position
        m_status = Error;
        m_position = -1;
    }
//    fprintf(stderr, "StringTerminatedSubStream::reset %lli %i\n", newpos, m_status);
    return m_position;
}

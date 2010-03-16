/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include <strigi/dataeventinputstream.h>
#include <strigi/strigiconfig.h>
#include <iostream>
#include <cassert>

using namespace std;
using namespace Strigi;

DataEventInputStream::DataEventInputStream(InputStream *i,
        DataEventHandler& h) :input(i), handler(h) {
    assert(input->position() == 0);
    m_size = input->size();
    totalread = 0;
    m_status = Ok;
    finished = false;
}
int32_t
DataEventInputStream::read(const char*& start, int32_t min, int32_t max) {
//    fprintf(stderr, "input->position(): %lli\n", input->position());
    int32_t nread = input->read(start, min, max);
//    fprintf(stderr, "%p pos: %lli min %i max %i nread %i\n", this, m_position, min, max, nread);
    if (nread < -1) {
        m_error = input->error();
        m_status = Error;
        return -2;
    }
    if (nread > 0) {
        m_position += nread;
        // value of -1 for totalread means data should not be reported anymore
        if (totalread != -1 && totalread < m_position) {
            int32_t amount = (int32_t)(m_position - totalread);
            bool wantMore = handler.handleData(start + nread - amount, amount);
            if (wantMore) {
                totalread = m_position;
            } else {
                totalread = -1;
            }
        }
    }
    if (nread < min) {
        m_status = Eof;
        if (m_size == -1) {
//            fprintf(stderr, "set m_size: %lli\n", m_position);
            m_size = m_position;
        }
#ifndef NDEBUG
        if (totalread != -1 && (m_size != m_position || m_size != totalread)) {
            cerr << "m_size: " << m_size << " m_position: " << m_position
                << " totalread: " << totalread << " nread: " << nread << endl;
            cerr << input->status() << " " << input->error() << endl;
        }
#endif
        assert(m_size == m_position);
        assert(totalread == -1 || totalread == m_size);
        if (!finished) {
            finish();
            finished = true;
        }
    }
    return nread;
}
int64_t
DataEventInputStream::skip(int64_t ntoskip) {
    int64_t skipped;
    if (totalread == -1) { // done reporting, we can use fast skipping
        skipped = input->skip(ntoskip);
        m_status = input->status();
        m_position = input->position();
        m_size = input->size();
    } else {
        // we call the default implementation because it calls
        // read() which is required for sending the signals
        skipped = InputStream::skip(ntoskip);
    }
    return skipped;
}
int64_t
DataEventInputStream::reset(int64_t np) {
//    fprintf(stderr, "DataEventInputStream::reset from %lli to %lli.\n", m_position, np);
    if (np > m_position) {
        // advance to the new position, using skip ensure we actually read
        // the files
        skip(np - m_position);
        return m_position;
    }
//    fprintf(stderr, "DataEventInputStream::reset\n");
    int64_t newpos = input->reset(np);
    if (newpos < 0) {
        m_status = Error;
        m_error = input->error();
    } else {
        m_status = (newpos == m_size) ?Eof :Ok;
    }
//    fprintf(stderr, "np %lli newpos %lli status %i\n", np, newpos, m_status);
    m_position = newpos;
    return newpos;
}
void
DataEventInputStream::finish() {
    handler.handleEnd();
}

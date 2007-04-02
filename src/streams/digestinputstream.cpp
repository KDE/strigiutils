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
#include "digestinputstream.h"
#include <cassert>

using namespace std;
using namespace Strigi;

DigestInputStream::DigestInputStream(InputStream *input) {
    assert(input->position() == 0);
    this->input = input;
    m_size = input->size();
    totalread = 0;
    m_status = Ok;
    finished = false;
    SHA1_Init(&sha1);
}
int32_t
DigestInputStream::read(const char*& start, int32_t min, int32_t max) {
    int32_t nread = input->read(start, min, max);
//    printf("%p pos: %lli min %i max %i nread %i\n", this, m_position, min, max, nread);
    if (nread < -1) {
        m_error = input->error();
        m_status = Error;
        return -2;
    }
    if (nread > 0) {
        m_position += nread;
    }
    if (totalread < m_position) {
        int32_t toSha1 = (int32_t)(m_position - totalread);
        SHA1_Update(&sha1, start+nread-toSha1, toSha1);
        totalread = m_position;
    }
    if (nread < min) {
        m_status = Eof;
        if (m_size == -1) {
//            printf("set m_size: %lli\n", m_position);
            m_size = m_position;
        }
#ifndef NDEBUG
        if (m_size != m_position || m_size != totalread) {
            fprintf(stderr, "m_size: %lli m_position: %lli totalread: %lli\n",
                m_size, m_position, totalread);
            fprintf(stderr, "%i %s\n", input->status(), input->error());
        }
        assert(m_size == m_position);
        assert(totalread == m_size);
#endif
        finishDigest();
    }
    return nread;
}
int64_t
DigestInputStream::skip(int64_t ntoskip) {
//    printf("skipping %lli\n", ntoskip);
    // we call the default implementation because it calls
    // read() which is required for updating the hash
    int64_t skipped = InputStream::skip(ntoskip);
    //const char*d;
    //int32_t skipped = read(d, ntoskip, ntoskip);
    return skipped;
}
int64_t
DigestInputStream::reset(int64_t np) {
//    printf("reset from %lli to %lli\n", m_position, np);
    if (np > m_position) {
        // advance to the new position, using skip ensure we actually read
        // the files
        skip(np - m_position);
        return m_position;
    }
    int64_t newpos = input->reset(np);
//    printf("np %lli newpos %lli\n", np, newpos);
    if (newpos < 0) {
        m_status = Error;
        m_error = input->error();
    } else {
        m_status = (newpos == m_size) ?Eof :Ok;
    }
    m_position = newpos;
    return newpos;
}
void
DigestInputStream::printDigest() {
    if (finished) {
        printf("The hash %p: %s\n", this, digestString().c_str());
    } else {
        printf("The digest is not ready yet.\n");
    }
}
void
DigestInputStream::finishDigest() {
    if (finished) return;
#ifndef NDEBUG
//    printf("finishing digest totalread: %lli m_size %lli\n", totalread, m_size);
    assert(totalread == m_size);
#endif
    SHA1_Final(digest, &sha1);
    finished = true;
#ifndef NDEBUG
//    printDigest();
#endif
}
string
DigestInputStream::digestString() {
    if (finished) {
        char d[41];
        for (int i = 0; i < 20; i++) {
            sprintf(d+2*i, "%02x", digest[i]);
        }
        return d;
    }
    return "";
}

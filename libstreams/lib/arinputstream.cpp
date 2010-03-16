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
#include <strigi/arinputstream.h>
#include <strigi/subinputstream.h>

#include <stdlib.h>
#include <cstring>

using namespace Strigi;

bool
ArInputStream::checkHeader(const char* data, int32_t datasize) {
    static const char magic[] = {0x21,0x3c,0x61,0x72,0x63,0x68,0x3e,0x0a};
    if (datasize < 8) return false;
    bool ok = std::memcmp(data, magic, 8) == 0;
    return ok;
}
ArInputStream::ArInputStream(InputStream* input)
        : SubStreamProvider(input) {
    // check header
    const char*b;
    if (input->read(b, 8, 8) != 8 || !checkHeader(b, 8)) {
        m_status = Error;
    }
}
ArInputStream::~ArInputStream() {
}
InputStream*
ArInputStream::nextEntry() {
    if (m_status) return 0;
    if (m_entrystream) {
        m_entrystream->skip(m_entrystream->size());
        delete m_entrystream;
        m_entrystream = 0;
    }
    readHeader();
    if (m_status) return 0;
    m_entrystream = new SubInputStream(m_input, m_entryinfo.size);
    return m_entrystream;
}
void
ArInputStream::readHeader() {
    const char *b;
    int32_t toread;
    int32_t nread;

    int64_t pos = m_input->position();
    if (pos%2) {
        m_input->skip(1);
    }

    // read the first 60 characters
    toread = 60;
    nread = m_input->read(b, toread, toread);
    if (m_input->status() == Error) {
        m_error = "Error reading ar header: ";
        m_error += m_input->error();
        m_status = Error;
        return;
    }
    if (nread <= 1) { // allow for a closing byte
        m_status = Eof;
        return;
    }
    if (nread != toread) {
        m_error = "Error reading ar header: premature end of file.";
        m_status = Error;
        return;
    }
    int len;
    for (len=0; len<16; len++) {
        char c = b[len];
        if (c == ' ' || c == '/' || c == '\0') {
            break;
        }
    }

    // we must copy this string to safely call atoi
    char bc[61];
    std::memcpy(bc, b, 60);
    bc[60] = '\0';
    m_entryinfo.size = atoi(bc+48);
    if (m_entryinfo.size < 0) {
        m_error = "Error: negative file size.";
        m_status = Error;
        return;
    }
    m_entryinfo.mtime = atoi(bc+16);
    if (len == 0) {
        if (b[1] == '/') {
            // fprintf(stderr, "SIZE: %lli\n", m_entryinfo.size);
            nread = m_input->read(b, (int32_t)m_entryinfo.size, (int32_t)m_entryinfo.size);
            if (nread != m_entryinfo.size) {
                m_error = "premature end of stream";
                m_status = Error;
                return;
            }
            gnufilenames.assign(b, (unsigned int)m_entryinfo.size);
            readHeader();
        } else if (b[1] == ' ') {
            m_input->skip(m_entryinfo.size);
            readHeader();
        } else {
            uint p = atoi(bc+1);
            if (gnufilenames.length() <= p) {
                m_error = "Invalid name field.";
                m_status = Error;
                return;
            }
            const char* c = gnufilenames.c_str() + p;
            const char* e = std::strchr(c, '/');
            if (e) {
                m_entryinfo.filename = std::string(c, e-c);
            } else {
                m_entryinfo.filename = c;
            }
        }
    } else {
        m_entryinfo.filename = std::string(b, len);
    }
    m_entryinfo.type = Strigi::EntryInfo::File;
}

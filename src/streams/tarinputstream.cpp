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
#include "tarinputstream.h"
#include <strigi/strigiconfig.h>
#include "subinputstream.h"
#include <cstring>

using namespace Strigi;

TarInputStream::TarInputStream(InputStream* input)
        : SubStreamProvider(input) {
}

TarInputStream::~TarInputStream() {
}
InputStream*
TarInputStream::nextEntry() {
    if (m_status) return 0;
    if (m_entrystream) {
        m_entrystream->skip(m_entrystream->size());
        m_input->skip(numPaddingBytes);
        delete m_entrystream;
        m_entrystream = 0;
    }
    parseHeader();
    if (m_status) return 0;
    m_entrystream = new SubInputStream(m_input, m_entryinfo.size);
    return m_entrystream;
}
const char*
TarInputStream::readHeader() {
    // read the first 500 characters
    const char *begin;
    int32_t nread = m_input->read(begin, 512, 512);
    if (nread != 512) {
        m_status = Error;
    }
    return begin;
}
bool
TarInputStream::checkHeader(const char* h, int32_t hsize) {
    if (hsize < 257) {
        // header is too small to check
        return false;
    }
    // the file starts with a filename of at most 100 characters. The filename
    // is ended by a \0, after this \0 only \0 is allowed
    int p = 0;
    while (p < 100 && h[p] != '\0') ++p;
    while (p < 100) {
        if (h[p++] != '\0') {
            return false;
        }
    }

    // check for field values that should be '\0' for the header to be a
    // tar header. Two positions are also accepted if they are ' ' because they
    return !(h[107] || h[115] || h[123] || (h[135]&&h[135]!=' ')
            || (h[147] && h[147] != ' ') || h[256]);
}
void
TarInputStream::parseHeader() {
    const char *hb;
    hb = readHeader();
    if (m_status) return;

    // check for terminators ('\0') on the first couple of fields
    if (!checkHeader(hb, 257)) {
        m_error = "Invalid tar header.\n";
        m_status = Error;
        return;
    }

    int32_t len = std::strlen(hb);
    if (len == 0) {
        // ready
        m_status = Eof;
        return;
    }
    if (len > 100) len = 100;
    m_entryinfo.filename.resize(0);
    size_t offset =  0;
    if (len > 2 && hb[0] == '.' && hb[1] == '/') {
        offset = 2; // skip initial './'
    }
    m_entryinfo.filename.append(hb, offset, len);
    if (m_entryinfo.filename == "././@LongLink") {
        m_entryinfo.filename.resize(0);
        readLongLink(hb);
        if (m_status) return;
        hb = readHeader();
        if (m_status) return;
    }

    // read the file size which is in octal format
    m_entryinfo.size = readOctalField(hb, 124);
    if (m_status) return;
    m_entryinfo.mtime = readOctalField(hb, 136);
    if (m_status) return;

    numPaddingBytes = 512 - m_entryinfo.size%512;
    if (numPaddingBytes == 512) {
        numPaddingBytes = 0;
    }

    len = m_entryinfo.filename.length();
    if (m_entryinfo.filename[len-1] == '/') {
        m_entryinfo.filename.resize(len-1);
    }
    // read file type
    if (hb[156] == 0 || hb[156] == '0') {
        m_entryinfo.type = EntryInfo::File;
    } else if (hb[156] == '5') {
        m_entryinfo.type = EntryInfo::Dir;
    } else {
        m_entryinfo.type = EntryInfo::Unknown;
    }
//    printf("!%s %i\n", m_entryinfo.filename.c_str(), hb[156]);
}
int32_t
TarInputStream::readOctalField(const char *b, int32_t offset) {
    int32_t val;
    int r = sscanf(b+offset, "%o", &val);
    if (r != 1) {
        m_status = Error;
        m_error = "Error reading header: octal field is not a valid integer.";
        return 0;
    }
    return val;
}
void
TarInputStream::readLongLink(const char *b) {
    int32_t toread = readOctalField(b, 124);
    int32_t left = toread%512;
    if (left) {
        left = 512 - left;
    }
    const char *begin;
    if (m_status) return;
    int32_t nread = m_input->read(begin, toread, toread);
    if (nread != toread) {
            m_status = Error;
            m_error = "Error reading LongLink: ";
            if (nread == -1) {
                m_error += m_input->error();
            } else {
                m_error += " premature end of file.";
            }
            return;
    }
    m_entryinfo.filename.append(begin, nread);

    int64_t skipped = m_input->skip(left);
    if (skipped != left) {
        m_status = Error;
        m_error = "Error reading LongLink: ";
        if (m_input->status() == Error) {
            m_error += m_input->error();
        } else {
            m_error += " premature end of file.";
        }
    }
}

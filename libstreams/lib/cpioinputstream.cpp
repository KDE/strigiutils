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
#include <strigi/cpioinputstream.h>
#include <strigi/strigiconfig.h>
#include <strigi/subinputstream.h>

#include <list>
#include <cstring>

using namespace std;
using namespace Strigi;

const char unsigned* CpioInputStream::magic = (const char unsigned *)"070701";

bool
CpioInputStream::checkHeader(const char* data, int32_t datasize) {
    if (datasize < 6) return false;
    // this check could be more strict
    bool ok = memcmp(data, magic, 6) == 0;
    return ok;
}
CpioInputStream::CpioInputStream(InputStream* input)
        : SubStreamProvider(input) {
    m_entryinfo.type = EntryInfo::File;
}
CpioInputStream::~CpioInputStream() {
}
InputStream*
CpioInputStream::nextEntry() {
    if (m_status) {
        delete m_entrystream;
        m_entrystream = 0;
        return 0;
    }
    if (m_entrystream) {
        while (m_entrystream->status() == Ok) {
            m_entrystream->skip(m_entrystream->size());
        }
        delete m_entrystream;
        m_entrystream = 0;
        if (padding) {
            m_input->skip(padding);
        }
    }
    readHeader();
    m_entrystream = new SubInputStream(m_input, m_entryinfo.size);
    if (m_status) {
        delete m_entrystream;
        m_entrystream = 0;
    }
    return m_entrystream;
}
void
CpioInputStream::readHeader() {
    //const unsigned char *hb;
    const char* b;
    int32_t toread;
    int32_t nread;

    // read the first 110 characters
    toread = 110;
    nread = m_input->read(b, toread, toread);
    if (nread != toread) {
        m_status = m_input->status();
        if (m_status == Eof) {
            return;
        }
        m_error = "Error reading cpio entry: ";
        if (nread == -1) {
            m_error += m_input->error();
        } else {
            m_error += " premature end of file.";
        }
        return;
    }
    // check header
    if (memcmp(b, magic, 6) != 0) {
        m_status = Error;
        m_error = "CPIO Entry signature is unknown: ";
        m_error.append(b, 6);
        return;
    }
    m_entryinfo.size = readHexField(b+54);
    m_entryinfo.mtime = readHexField(b+46);
    int32_t filenamesize = readHexField(b+94);
    if (m_status) {
        m_error = "Error parsing entry field.";
        return;
    }
    char namepadding = (char)((filenamesize+2) % 4);
    if (namepadding) namepadding = (char)(4 - namepadding);
    padding = (char)(m_entryinfo.size % 4);
    if (padding) padding = (char)(4 - padding);

    // read filename
    toread = filenamesize + namepadding;
    nread = m_input->read(b, toread, toread);
    if (nread != toread) {
        m_error = "Error reading cpio entry name.";
        m_status = Error;
        return;
    }
    int32_t len = filenamesize - 1;
    if (len > 2 && b[0] == '.' && b[1] == '/') {
        b += 2;
    }
    // check if the name is not shorter than specified
    len = 0;
    while (len < filenamesize && b[len] != '\0') len++;
    m_entryinfo.filename = std::string((const char*)b, len);

    // if an cpio has an entry 'TRAILER!!!' we are at the end
    if ("TRAILER!!!" == m_entryinfo.filename) {
        m_status = Eof;
    }
}
int32_t
CpioInputStream::readHexField(const char *b) {
    int32_t val = 0;
    char c;
    for (unsigned char i=0; i<8; ++i) {
        val <<= 4;
        c = b[i];
        if (c > 'F') {
            val += c - 87;
        } else if (c > '9') {
            val += c - 55;
        } else {
            val += c - 48;
        }
    }
    return val;
}

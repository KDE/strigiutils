/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Alexandr Goncearenco <neksa@neksa.net>
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
#include <strigi/sdfinputstream.h>
#include <strigi/subinputstream.h>
#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;
using namespace Strigi;

const string SdfInputStream::delimiter("$$$$");
const string SdfInputStream::label("V2000");

SdfInputStream::SdfInputStream(InputStream* input)
        : SubStreamProvider(input), substream(0), entrynumber(0),
        previousStartOfDelimiter(0) {
    m_searcher.setQuery(delimiter);
}
SdfInputStream::~SdfInputStream() {
    if (substream && substream != m_entrystream) {
        delete substream;
    }
}
/**
 * Return a pointer that points past the current line. The line can end on
 * either \r, \n, or \r\n.
 * If the pointer would point past then end of the buffer or the line is longer
 * than 80 bytes, NULL is returned.
 **/
const char*
skip80Line(const char* data, int32_t size) {
    if (size <= 0) return 0;
    // EOL can be at position 80
    int max = min(size, 81);
    int i;
    for (i=0; i<max && data[i] != '\n' && data[i] != '\r'; ++i) {
    }
    if (i == max) return 0;
    if (data[i] == '\n') return (i+1 < size) ?data+i+1 :0;
    // data[i] is known to be '\r' by now
    if (i+1 < size && data[i+1] != '\n') return data+i+1;
    // data[i+1] is known to be '\n' by now
    return (i+2 < size) ?data+i+2 :0;
}
bool
SdfInputStream::checkHeader(const char* data, int32_t datasize) {
    if (datasize <= 10) return false;
    // the fourth line must contain the string "V2000"
    // skip three lines of at most 80 bytes
    const char* thisLine = 0;
    const char* nextLine = data;
    for (int i=0; i<4 && nextLine != 0; ++i) {
        thisLine = nextLine;
        nextLine = skip80Line(thisLine, datasize - (int32_t)(thisLine - data));
    }
    static const KmpSearcher searcher(label);
    if (nextLine != 0) {
        nextLine = searcher.search(thisLine, (int32_t)(nextLine - thisLine));
    }
    return nextLine != 0;
}
InputStream*
SdfInputStream::nextEntry() {
    if (m_status != Ok) return 0;

    m_input->reset(previousStartOfDelimiter);
    int32_t nread = 0;

    // read anything that's left over in the previous substream
    if (substream) {
        substream->reset(0);
        const char* dummy;
        while (substream->status() == Ok) {
            substream->read(dummy, 1, 0);
        }
        if (substream->status() == Error) {
            m_status = Error;
        }
        if (substream && substream != m_entrystream) {
            delete substream;
        }
        substream = 0;
        delete m_entrystream;
        m_entrystream = 0;
    
        m_input->reset(previousStartOfDelimiter);
    
        // eat delimiter and following newlines
        if (m_input->status() == Ok) {
            nread = m_input->read(dummy, 4, 4);
            if (nread == 4 && strncmp(dummy, delimiter.c_str(), 4) == 0) {
                m_input->read(dummy, 1, 1);
                while (m_input->status() == Ok && (
                        strncmp(dummy, "\n", 1) == 0
                        || strncmp(dummy, "\r", 1) == 0)) {
                    m_input->read(dummy, 1, 1);
                }
            }
        }
    }
    // make sure it is not a MOL
    // we can not check it in checkHeader due to low header size limit
    // There is only one way to destinguish between MOL and SD:
    // MOL does not have $$$$ delimiter. Return no entries if it is a MOL.
    const char* start;
    const char* end;
    int32_t total = 0;
    const int64_t pos = m_input->position();
    int64_t len=0;
    
    while (m_input->status() == Ok) {
        nread = m_input->read(start, 1024, 0);
        if (nread > 0) {
            end = m_searcher.search(start, nread);
            if (end) {
                len = end - start + total;
                break;
            }
            total += nread;
        }    
    }    
    if (m_input->status() == Error) {
        m_status = Error;
        m_entrystream = 0;
        return 0;
    }
    
    m_input->reset(pos);
    
    if (len > 0) {
        // this stream is an SD
        substream = new SubInputStream(m_input, len);
        previousStartOfDelimiter = m_input->position() + len;
        m_entryinfo.type = EntryInfo::File;
        m_entryinfo.size = len;

        m_entryinfo.filename.assign("Molecule");
        entrynumber++;
        ostringstream o;
        o << entrynumber;
        m_entryinfo.filename.append(o.str());

        m_entrystream = substream;
        return m_entrystream;
    } else {
        // this stream is a MOL itself, not an SD
        m_status = Eof;
        m_entrystream = 0;
        return 0;
    }
}

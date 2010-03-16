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
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <strigi/encodinginputstream.h>
#include <iconv.h>
#include <cerrno>
using namespace Strigi;

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
     #define ICONV_CONST const
#else
     #define ICONV_CONST
#endif

class EncodingInputStream::Private {
public:
    StreamBuffer<char> charbuf;
    EncodingInputStream* const p;
    InputStream* input;
    iconv_t converter;
    int32_t charsLeft;
    bool finishedEncoding;

    Private(EncodingInputStream* eis, InputStream* i, const char* inenc,
            const char* outenc) :p(eis), input(i),
            converter((iconv_t)-1), charsLeft(0), finishedEncoding(false) {
        if (outenc == 0) {
            outenc = "UTF-8";
        }
        converter = iconv_open(outenc, inenc);
    }
    ~Private() {
        if (converter != (iconv_t) -1) {
            iconv_close(converter);
        }
    }
    int32_t decode(char* start, int32_t space);
};

EncodingInputStream::EncodingInputStream(InputStream* s, const char* inenc,
        const char* outenc)
        :p(new Private(this, s, inenc, outenc)) {
    if (inenc == 0) {
        m_status = Error;
        m_error = "No input encoding provided.";
        return;
    }
    m_status = Ok;

    // check if the converter is valid
    if (p->converter == (iconv_t) -1) {
        m_error = "conversion from '";
        m_error.append(inenc);
        m_error.append("' to '");
        m_error.append(outenc ? outenc : "(null)");
        m_error.append(" not available.");
        m_status = Error;
        return;
    }
    p->charbuf.setSize(262);
    p->charsLeft = 0;
}
EncodingInputStream::~EncodingInputStream() {
    delete p;
}
int32_t
EncodingInputStream::Private::decode(char* start, int32_t space) {
    // decode from charbuf
    ICONV_CONST char *inbuf = charbuf.readPos;
    size_t inbytesleft = charbuf.avail;
    size_t outbytesleft = space;
    char *outbuf = start;
    size_t r = iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    int32_t nwritten;
    if (r == (size_t)-1) {
        switch (errno) {
        case EILSEQ: //invalid multibyte sequence
            p->m_error = "Invalid multibyte sequence.";
            p->m_status = Error;
            return -1;
        case EINVAL: // last character is incomplete
            // move from inbuf to the end to the start of
            // the buffer
            std::memmove(charbuf.start, inbuf, inbytesleft);
            charbuf.readPos = charbuf.start;
            charbuf.avail = (int32_t)inbytesleft;
            nwritten = (int32_t)(outbuf - start);
            break;
        case E2BIG: // output buffer is full
            charbuf.readPos += charbuf.avail - inbytesleft;
            charbuf.avail = (int32_t)inbytesleft;
            nwritten = space;
            break;
        default:
            char tmp[10];
            snprintf(tmp, 10, "%i", errno);
            p->m_error = "inputstreamreader error: ";
            p->m_error.append(tmp);
            fprintf(stderr, "inputstreamreader::error %d\n", errno);
            p->m_status = Error;
            return -1;
        }
    } else { //input sequence was completely converted
        charbuf.readPos = charbuf.start;
        charbuf.avail = 0;
        nwritten = (int32_t)(outbuf - start);
        if (input == 0) {
            finishedEncoding = true;
        }
    }
    return nwritten;
}
int32_t
EncodingInputStream::fillBuffer(char* start, int32_t space) {
    // fill up charbuf
    if (p->input && p->charbuf.readPos == p->charbuf.start) {
        const char *begin;
        int32_t numRead;
        numRead = p->input->read(begin, 1, p->charbuf.size - p->charbuf.avail);
        //printf("filled up charbuf\n");
        if (numRead < -1) {
            m_error = p->input->error();
            m_status = Error;
            p->input = 0;
            return numRead;
        }
        if (numRead < 1) {
            // signal end of input buffer
            p->input = 0;
            if (p->charbuf.avail) {
                m_error = "stream ends on incomplete character";
                m_status = Error;
            }
            return -1;
        }
        // copy data into other buffer
        std::memmove(p->charbuf.start + p->charbuf.avail, begin, numRead);
        p->charbuf.avail = numRead + p->charbuf.avail;
    }
    // decode
    int32_t n = p->decode(start, space);
    //printf("decoded %i\n", n);
    return n;
}

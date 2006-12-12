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
#include "jstreamsconfig.h"
#include "inputstreamreader.h"
#include "jstreamsconfig.h"
#include <cerrno>
using namespace jstreams;

#ifndef ICONV_CONST
     //we try to guess whether the iconv function requires
     //a const char. We have no way of automatically figuring
     //this out if we did not use autoconf, so we guess based
     //on certain parameters:
     #if defined (_LIBICONV_H)
          #define ICONV_CONST const
     #else
          #define ICONV_CONST
     #endif
#endif

InputStreamReader::InputStreamReader(StreamBase<char>* i, const char* enc) {
    status = Ok;
    finishedDecoding = false;
    input = i;
    if (enc == 0) enc = "UTF-8";
#ifdef _LIBICONV_H
    if (sizeof(wchar_t) == 4) {
        converter = iconv_open("UCS-4-INTERNAL", enc);
    } if (sizeof(wchar_t) == 2) {
        converter = iconv_open("UCS-2-INTERNAL", enc);
#else
    if (sizeof(wchar_t) > 1) {
        converter = iconv_open("WCHAR_T", enc);
#endif
    } else {
        converter = iconv_open("ASCII", enc);
    }

    // check if the converter is valid
    if (converter == (iconv_t) -1) {
        error = "conversion from '";
        error += enc;
        error += "' not available.";
        status = Error;
        return;
    }
    charbuf.setSize(262);
    //mark(262);
    charsLeft = 0;
}
InputStreamReader::~InputStreamReader() {
    if (converter != (iconv_t) -1) {
        iconv_close(converter);
    }
}
int32_t
InputStreamReader::decode(wchar_t* start, int32_t space) {
    // decode from charbuf
    ICONV_CONST char *inbuf = charbuf.readPos;
    size_t inbytesleft = charbuf.avail;
    size_t outbytesleft = sizeof(wchar_t)*space;
    char *outbuf = (char*)start;
    size_t r = iconv(converter, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    int32_t nwritten;
    if (r == (size_t)-1) {
        switch (errno) {
        case EILSEQ: //invalid multibyte sequence
            error = "Invalid multibyte sequence.";
            status = Error;
            return -1;
        case EINVAL: // last character is incomplete
            // move from inbuf to the end to the start of
            // the buffer
            memmove(charbuf.start, inbuf, inbytesleft);
            charbuf.readPos = charbuf.start;
            charbuf.avail = inbytesleft;
            nwritten = ((wchar_t*)outbuf) - start;
            break;
        case E2BIG: // output buffer is full
            charbuf.readPos += charbuf.avail - inbytesleft;
            charbuf.avail = inbytesleft;
            nwritten = space;
            break;
        default:
            char tmp[10];
            snprintf(tmp, 10, "%i", errno);
            error = "inputstreamreader error: ";
            error.append(tmp);
            fprintf(stderr, "inputstreamreader::error %d\n", errno);
            status = Error;
            return -1;
        }
    } else { //input sequence was completely converted
        charbuf.readPos = charbuf.start;
        charbuf.avail = 0;
        nwritten = ((wchar_t*)outbuf) - start;
        if (input == 0) {
            finishedDecoding = true;
        }
    }
    return nwritten;
}
int32_t
InputStreamReader::fillBuffer(wchar_t* start, int32_t space) {
    // fill up charbuf
    if (input && charbuf.readPos == charbuf.start) {
        const char *begin;
        int32_t numRead;
        numRead = input->read(begin, 1, charbuf.size - charbuf.avail);
        //printf("filled up charbuf\n");
        if (numRead < -1) {
            error = input->getError();
            status = Error;
            input = 0;
            return numRead;
        }
        if (numRead < 1) {
            // signal end of input buffer
            input = 0;
            if (charbuf.avail) {
                error = "stream ends on incomplete character";
                status = Error;
            }
            return -1;
        }
        // copy data into other buffer
        memmove(charbuf.start + charbuf.avail, begin, numRead);
        charbuf.avail = numRead + charbuf.avail;
    }
    // decode
    int32_t n = decode(start, space);
    //printf("decoded %i\n", n);
    return n;
}

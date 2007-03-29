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
#include "stringterminatedsubstream.h"
using namespace jstreams;

// TODO add a mechanism that avoid searching for a stop point again after a
// reset

int32_t
StringTerminatedSubStream::read(const char*& start, int32_t min, int32_t max) {
    if (status == Eof) return -1;
    if (status == Error) return -2;

    // convenience parameter
    int32_t tl = searcher.getQueryLength();

    // increase min and max to accommodate for the length of the terminator
    int32_t tlmin = min;
    int32_t tlmax = max;
    if (tlmin == 0) {
        tlmin = 1 + tl;
    } else {
        tlmin += tl;
    }
    if (tlmax > 0 && tlmax < tlmin) tlmax = tlmin;

    int64_t pos = input->getPosition();
    int32_t nread = input->read(start, tlmin, tlmax);
    //printf("str %i pos %lli offset %lli\n", nread, pos, offset);
    if (nread == -1) {
        status = Eof;
        return nread;
    }
    if (nread < -1) {
        status = Error;
        error = input->getError();
        return nread;
    }

    const char* end = searcher.search(start, nread);
    if (end) {
        // the end signature was found
        //printf("THE END %p %p %s\n", start, end, searcher.getQuery().c_str());
        //printf("TE %i '%.*s'\n", end-start, 10, end);
        nread = end - start;
        // signal the end of stream at the next call
        status = Eof;
        // set input stream to point after the terminator
        input->reset(pos + nread + tl);
    } else if (nread > tlmin) {
        // we are not at or near the end and read the required amount
        // reserve the last bit of buffer for rereading to match the terminator
        // in the next call
        nread -= tl;
        // we rewind, but the pointer 'start' will stay valid nontheless
        input->reset(pos + nread);
    } else if (max != 0 && nread > max) {
        // we are near the end of the stream but cannot pass all data
        // at once because the amount read is larger than the amount to pass
        input->reset(pos + max);
        nread = max;
    } else {
        // we are at the end of the stream, so no need to rewind
        // signal the end of stream at the next call
        status = Eof;
    }
//    printf("stss: %i '%.*s'\n", nread, (20>nread)?nread:20, start);
    if (nread > 0) position += nread;
    if (status == Eof) {
//        printf("Eof size %lli pos %lli\n", size, position);
        size = position;
    }
//    printf("str2 %i\n", nread);
    return nread;
}
/*int64_t
StringTerminatedSubStream::mark(int32_t readlimit) {
//    printf("mark %i\n", readlimit);
    return input->mark(readlimit) - offset;
}*/
int64_t
StringTerminatedSubStream::reset(int64_t newpos) {
//    printf("stssreset %lli\n", newpos);
    position = input->reset(newpos+offset);
    if (position >= offset) {
        position -= offset;
        if (position != size) status = Ok;
    } else {
        // the stream is not positioned at a valid position
        status = Error;
        position = -1;
    }
    return position;
}

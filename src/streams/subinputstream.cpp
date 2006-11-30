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
#include "subinputstream.h"
#include <cassert>
using namespace jstreams;

SubInputStream::SubInputStream(StreamBase<char> *i, int64_t length)
        : offset(i->getPosition()), input(i) {
    assert(length >= -1);
//    printf("substream offset: %lli\n", offset);
    size = length;
}
int32_t
SubInputStream::read(const char*& start, int32_t min, int32_t max) {
    if (size != -1) {
        const int64_t left = size - position;
        if (left == 0) {
            return -1;
        }
        // restrict the amount of data that can be read
        if (max <= 0 || max > left) {
            max = (int32_t)left;
        }
        if (min > max) min = max;
        if (left < min) min = (int32_t)left;
    }
    int32_t nread = input->read(start, min, max);
    if (nread < -1) {
        fprintf(stderr, "substream too short.\n");
        status = Error;
        error = input->getError();
    } else if (nread < min) {
        if (size == -1) {
            status = Eof;
            if (nread > 0) {
                position += nread;
                size = position;
            }
        } else {
//            fprintf(stderr, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! nread %i min %i max %i size %lli\n", nread, min, max, size);
//            fprintf(stderr, "pos %lli parentpos %lli\n", position, input->getPosition());
//            fprintf(stderr, "status: %i error: %s\n", input->getStatus(), input->getError());
            // we expected data but didn't get enough so that's an error
            status = Error;
            error = "Premature end of stream\n";
            nread = -2;
        }
    } else {
        position += nread;
        if (position == size) {
            status = Eof;
        }
    }
    return nread;
}
int64_t
SubInputStream::reset(int64_t newpos) {
    assert(newpos >= 0);
//    fprintf(stderr, "subreset pos: %lli newpos: %lli offset: %lli\n", position,
//        newpos, offset);
    position = input->reset(newpos + offset);
    if (position < offset) {
        fprintf(stderr, "########### position %lli newpos %lli\n", position, newpos);
        status = Error;
        error = input->getError();
    } else {
        position -= offset;
        status = input->getStatus();
    }
    return position;
}
int64_t
SubInputStream::skip(int64_t ntoskip) {
//    printf("subskip pos: %lli ntoskip: %lli offset: %lli\n", position, ntoskip, offset);
    if (size == position) {
        status = Eof;
        return -1;
    }
    if (size != -1) {
        const int64_t left = size - position;
        // restrict the amount of data that can be skipped
        if (ntoskip > left) {
            ntoskip = left;
        }
    }
    int64_t skipped = input->skip(ntoskip);
    if (input->getStatus() == Error) {
        status = Error;
        error = input->getError();
    } else {
        position += skipped;
        if (position == size) {
            status = Eof;
        } else if (skipped <= 0) {
            status = Error;
            error = "Premature end of stream\n";
            skipped = -2;
        }
    }
    return skipped;
}

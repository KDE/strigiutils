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
#include "jstreamsconfig.h"
#include "dataeventinputstream.h"
#include <cassert>
using namespace std;
using namespace jstreams;

DataEventInputStream::DataEventInputStream(StreamBase<char> *i) :input(i) {
    assert(input->getPosition() == 0);
    size = input->getSize();
    totalread = 0;
    status = Ok;
}
int32_t
DataEventInputStream::read(const char*& start, int32_t min, int32_t max) {
    int32_t nread = input->read(start, min, max);
//    printf("%p pos: %lli min %i max %i nread %i\n", this, position, min, max, nread);
    if (nread < -1) {
        error = input->getError();
        status = Error;
        return -2;
    }
    if (nread > 0) {
        position += nread;
    }
    if (totalread < position) {
        int32_t amount = (int32_t)(position - totalread);
        list<DataEventHandler*>::iterator i;
        for (i = handlers.begin(); i!= handlers.end(); ++i) {
            (*i)->handleData(start + nread - amount, amount);
        }
        totalread = position;
    }
    if (nread < min) {
        status = Eof;
        if (size == -1) {
//            printf("set size: %lli\n", position);
            size = position;
        }
#ifndef NDEBUG
        if (size != position || size != totalread) {
            fprintf(stderr, "size: %lli position: %lli totalread: %lli\n",
                size, position, totalread);
            fprintf(stderr, "%i %s\n", input->getStatus(), input->getError());
        }
#endif
        assert(size == position);
        assert(totalread == size);
        finish();
    }
    return nread;
}
int64_t
DataEventInputStream::skip(int64_t ntoskip) {
//    printf("skipping %lli\n", ntoskip);
    // we call the default implementation because it calls
    // read() which is required for sending the signals
    int64_t skipped = StreamBase<char>::skip(ntoskip);
    //const char*d;
    //int32_t skipped = read(d, ntoskip, ntoskip);
    return skipped;
}
int64_t
DataEventInputStream::reset(int64_t np) {
//    printf("reset from %lli to %lli\n", position, np);
    if (np > position) {
        // advance to the new position, using skip ensure we actually read
        // the files
        skip(np - position);
        return position;
    }
    int64_t newpos = input->reset(np);
//    printf("np %lli newpos %lli\n", np, newpos);
    if (newpos < 0) {
        status = Error;
        error = input->getError();
    } else {
        status = (newpos == size) ?Eof :Ok;
    }
    position = newpos;
    return newpos;
}
void
DataEventInputStream::finish() {
    list<DataEventHandler*>::iterator i;
    for (i = handlers.begin(); i!= handlers.end(); ++i) {
        (*i)->handleEnd();
    }
}

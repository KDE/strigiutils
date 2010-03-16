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
#include <strigi/strigiconfig.h>
#include "inputstreamtests.h"
#include <strigi/substreamprovider.h>
#include <iostream>

using namespace Strigi;

int founderrors = 0;

template <class T>
void
inputStreamTest1(StreamBase<T>* s) {
    int64_t size = s->size();
    VERIFY(s->skip(0) == 0);
    VERIFY(size >= -1);
    const T* ptr = 0;
    int32_t n;
    if (size == -1) {
        n = 0;
        int32_t n2 = s->read(ptr, 1, 0);
        if (s->status() == Error) fprintf(stderr, "%s\n", s->error());
        VERIFY(n2 > -2);
        VERIFY(ptr);
        while (n2 > 0) {
            n += n2;
            n2 = s->read(ptr, 1, 0);
            if (s->status() == Error) fprintf(stderr, "%s\n", s->error());
            VERIFY(n2 > -2);
            VERIFY(ptr);
        }
        size = s->size();
    } else {
        // read past the end
        n = s->read(ptr, (int32_t)size+1, (int32_t)size+1);
        VERIFY(ptr);
    }
    VERIFY(size == n);
    VERIFY(s->position() == n);
    if (s->status() == Strigi::Error) {
        fprintf(stderr, "error %s\n", s->error());
    }
    VERIFY(s->status() == Strigi::Eof);
}

template <class T>
void
inputStreamTest2(StreamBase<T>* s) {
    int64_t p = s->position();
    const T* ptr;
    int64_t n = s->read(ptr, 100, 0);
    VERIFY(n > 0);
    n = s->reset(p);
    VERIFY(n == p);
    VERIFY(s->position() == p);
    inputStreamTest1(s);
}

template <class T>
void
inputStreamTest3(StreamBase<T>* s) {
    // read beyond end, then reset, check status and try to skip so much again
    int64_t p = s->position();
    int32_t toread = (int32_t)s->size();
    toread = (toread > 0) ?toread+100 :1000000;
    const T* ptr;
    int64_t n = s->read(ptr, toread, 0);
    VERIFY(n == s->size());
    n = s->reset(p);
    VERIFY(n == p);
    VERIFY(s->status() == Ok);

    int64_t n2 = s->skip(toread);
    VERIFY(n2 == s->size());
}

void
subStreamProviderTest1(SubStreamProvider* ssp) {
    StreamBase<char>* s = ssp->nextEntry();
    VERIFY(s == ssp->currentEntry());
    VERIFY(s); // we want at least one stream
    while (s) {
        inputStreamTest1<char>(s);
        printf("%s %li\n", ssp->entryInfo().filename.c_str(),
            (long)ssp->entryInfo().size);
        s = ssp->nextEntry();
        VERIFY(s == ssp->currentEntry());
    }
    if (ssp->status() == Strigi::Error) {
        fprintf(stderr, "%s\n", ssp->error());
    }
    VERIFY(ssp->status() == Strigi::Eof);
}
void
subStreamProviderTest2(SubStreamProvider* ssp) {
    StreamBase<char>* s = ssp->nextEntry();
    VERIFY(s == ssp->currentEntry());
    while (s) {
        inputStreamTest2<char>(s);
        s = ssp->nextEntry();
        VERIFY(s == ssp->currentEntry());
    }
    if (ssp->status() == Strigi::Error) {
        fprintf(stderr, "%s\n", ssp->error());
    }
    VERIFY(ssp->status() == Strigi::Eof);
}

int ninputstreamtests = 3;
void (*charinputstreamtests[])(StreamBase<char>*) = {
    inputStreamTest1, inputStreamTest2, inputStreamTest3 };
void (*wcharinputstreamtests[])(StreamBase<wchar_t>*) = {
    inputStreamTest1, inputStreamTest2, inputStreamTest3 };

int nstreamprovidertests = 2;
void (*streamprovidertests[])(SubStreamProvider*) = {
    subStreamProviderTest1, subStreamProviderTest2 };

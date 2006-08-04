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
#include "inputstreamtests.h"
#include "../substreamprovider.h"
using namespace jstreams;

int founderrors = 0;

template <class T>
void
inputStreamTest1(StreamBase<T>* s) {
    int64_t size = s->getSize();
    const T* ptr;
    int32_t n;
    if (size == -1) {
        n = 0;
        int32_t n2 = s->read(ptr, 1, 0);
        while (n2 > 0) {
            n += n2;
            n2 = s->read(ptr, 1, 0);
        }
    } else {
        // read past the end
        n = s->read(ptr, size+1, size+1);
    }
    VERIFY(size == -1 || n == -1 || size == n);
    VERIFY(s->getPosition() == n);
    if (s->getStatus() == jstreams::Error) {
        fprintf(stderr, "error %s\n", s->getError());
    }
    VERIFY(s->getStatus() == jstreams::Eof);
}

template <class T>
void
inputStreamTest2(StreamBase<T>* s) {
    int64_t p = s->getPosition();
    const T* ptr;
    int64_t n = s->read(ptr, 100, 0);
    VERIFY(n > 0);
    n = s->reset(p);
    n = s->skip(100);
    VERIFY(n > 0);
    n = s->reset(p);
    VERIFY(n == p);
    VERIFY(s->getPosition() == p);
    inputStreamTest1(s);
}
void
subStreamProviderTest1(SubStreamProvider* ssp) {
    StreamBase<char>* s = ssp->nextEntry();
    while (s) {
        inputStreamTest1<char>(s);
        s = ssp->nextEntry();
    }
    if (ssp->getStatus() == jstreams::Error) {
        fprintf(stderr, "%s\n", ssp->getError());
    }
    VERIFY(ssp->getStatus() == jstreams::Eof);
}
void
subStreamProviderTest2(SubStreamProvider* ssp) {
    StreamBase<char>* s = ssp->nextEntry();
    while (s) {
        inputStreamTest2<char>(s);
        s = ssp->nextEntry();
    }
    if (ssp->getStatus() == jstreams::Error) {
        fprintf(stderr, "%s\n", ssp->getError());
    }
    VERIFY(ssp->getStatus() == jstreams::Eof);
}

int ninputstreamtests = 2;
void (*charinputstreamtests[])(StreamBase<char>*) = {
    inputStreamTest1, inputStreamTest2 };
void (*wcharinputstreamtests[])(StreamBase<wchar_t>*) = {
    inputStreamTest1, inputStreamTest2 };

int nstreamprovidertests = 2;
void (*streamprovidertests[])(SubStreamProvider*) = {
    subStreamProviderTest1, subStreamProviderTest2 };

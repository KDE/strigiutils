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
#ifndef STRIGI_INPUTSTREAMTESTS
#define STRIGI_INPUTSTREAMTESTS
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace Strigi {
    template <class T> class StreamBase;
    class SubStreamProvider;
} // end namespace Strigi

template <class T>
void inputStreamTest1(Strigi::StreamBase<T>* stream);

template <class T>
void inputStreamTest2(Strigi::StreamBase<T>* stream);

void subStreamProviderTest1(Strigi::SubStreamProvider* stream);

extern int ninputstreamtests;
extern void (*charinputstreamtests[])(Strigi::StreamBase<char>*);
extern void (*wcharinputstreamtests[])(Strigi::StreamBase<wchar_t>*);

extern int nstreamprovidertests;
extern void (*streamprovidertests[])(Strigi::SubStreamProvider*);

extern int founderrors;
#define VERIFY(TESTBOOL) if (!(TESTBOOL)) {\
	fprintf(stderr, "test '%s' failed at\n\t%s:%i\n", \
		#TESTBOOL, __FILE__, __LINE__); \
	founderrors++; \
}

#include "unknownsizestream.h"
#include <strigi/fileinputstream.h>

#define TESTONFILE(CLASS, FILE)  \
    for (int i=0; i<ninputstreamtests; ++i) { \
        InputStream* f = FileInputStream::open(FILE); \
        CLASS s1(f); \
        charinputstreamtests[i](&s1); \
        delete f; \
\
        f = FileInputStream::open(FILE, FileInputStream::Buffered); \
        CLASS s2(f); \
        charinputstreamtests[i](&s2); \
        delete f; \
\
        f = FileInputStream::open(FILE); \
        UnknownSizeInputStream u3(f); \
        CLASS s3(&u3); \
        charinputstreamtests[i](&s3); \
        delete f; \
\
        f = FileInputStream::open(FILE, FileInputStream::Buffered); \
        UnknownSizeInputStream u4(f); \
        CLASS s4(&u4); \
        charinputstreamtests[i](&s4); \
        delete f; \
    }

#define TESTONFILE2(CLASS, ARG, FILE)  \
    for (int i=0; i<ninputstreamtests; ++i) { \
        InputStream* f = FileInputStream::open(FILE); \
        CLASS s1(f, ARG); \
        charinputstreamtests[i](&s1); \
        delete f; \
\
        f = FileInputStream::open(FILE, FileInputStream::Buffered); \
        CLASS s2(f, ARG); \
        charinputstreamtests[i](&s2); \
        delete f; \
\
        f = FileInputStream::open(FILE); \
        UnknownSizeInputStream u3(f); \
        CLASS s3(&u3, ARG); \
        charinputstreamtests[i](&s3); \
        delete f; \
\
        f = FileInputStream::open(FILE, FileInputStream::Buffered); \
        UnknownSizeInputStream u4(f); \
        CLASS s4(&u4, ARG); \
        charinputstreamtests[i](&s4); \
        delete f; \
    }

#define TESTONARCHIVE(CLASS, FILE)  \
    for (int i=0; i<nstreamprovidertests; ++i) { \
        InputStream* f = FileInputStream::open(FILE); \
        CLASS s1(f); \
        streamprovidertests[i](&s1); \
        delete f; \
\
        f = FileInputStream::open(FILE, FileInputStream::Buffered); \
        CLASS s2(f); \
        streamprovidertests[i](&s2); \
        delete f; \
\
        f = FileInputStream::open(FILE); \
        UnknownSizeInputStream u3(f); \
        CLASS s3(&u3); \
        streamprovidertests[i](&s3); \
        delete f; \
\
        f = FileInputStream::open(FILE, FileInputStream::Buffered); \
        UnknownSizeInputStream u4(f); \
        CLASS s4(&u4); \
        streamprovidertests[i](&s4); \
        delete f; \
    }

#endif

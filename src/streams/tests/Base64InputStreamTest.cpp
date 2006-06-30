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
#include "Base64InputStreamTest.h"
#include "../base64inputstream.h"
#include "../fileinputstream.h"
#include "inputstreamtests.h"
using namespace jstreams;

void
Base64InputStreamTest::testStream() {
    FileInputStream file("base64enc.txt");
    Base64InputStream b64(&file);
    b64.mark(1);
    const char* start;
    int32_t nread = b64.read(start, 1, 0);
    while (nread > 0) {
        //printf("%i\n", nread);
        for (int i=0; i<nread; ++i) {
            printf("%c", start[i]);
        }
        nread = b64.read(start, 1, 0);
    }
    printf("\n");
/*    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.zip");
        charinputstreamtests[i](&file);
    }*/
}


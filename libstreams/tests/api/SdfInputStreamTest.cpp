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
#include <strigi/fileinputstream.h>
#include <strigi/sdfinputstream.h>
#include "../sharedtestcode/inputstreamtests.h"

using namespace Strigi;

void
testSdfHeader(const char* filename) {
    FileInputStream file(filename);
    const char* header;
    int32_t nread = file.read(header, 1024, 1024);
    bool ok = nread > 0 && SdfInputStream::checkHeader(header, nread);
    if (ok) {
        file.reset(0);
        printf("SDF %s\n", (const char*)filename);
    }
    VERIFY(ok == true);
}

int
SdfInputStreamTest(int argc, char* argv[]) {
    if (argc < 2) return 1;
    VERIFY(chdir(argv[1]) == 0);
    founderrors = 0;
    
    testSdfHeader("drugs.sdf");

    TESTONARCHIVE(SdfInputStream, "drugs.sdf");
    return founderrors;
}


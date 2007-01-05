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
#include "processinputstream.h"
#include "stringreader.h"
using namespace jstreams;

int
main(int argc, char** argv) {
    std::vector<std::string> a;
    for (int i=1; i<argc; ++i) {
        a.push_back(argv[i]);
    }

    ProcessInputStream x(a);
    const char* d;
    int32_t n = x.read(d, 1, 1);
    while (n > 0) {
        printf("%.*s", n, d);
        n = x.read(d, 1, 1);
    }

    std::vector<std::string> b;
    b.push_back("/bin/cat");
    StringInputStream s("hallo\n");
    ProcessInputStream y(b, &s);
    n = y.read(d, 1, 1);
    while (n > 0) {
        printf("%.*s", n, d);
        n = y.read(d, 1, 1);
    }


    return 0;
}

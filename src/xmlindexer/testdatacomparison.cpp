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
#include "fileinputstream.h"
#include "bz2inputstream.h"
#include "indexer.h"
#include "filtermanager.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <sstream>
using namespace jstreams;
using namespace std;

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s [dir-to-index]\n", argv[0]);
}
bool
containsHelp(int argc, char **argv) {
    for (int i=1; i<argc; ++i) {
         if (strcmp(argv[i], "--help") == 0
             || strcmp(argv[i], "-h") == 0) return true;
    }
    return false;
}
int
main(int argc, char** argv) {
    if (containsHelp(argc, argv) || (argc != 3)) {
        printUsage(argv);
        return -1;
    }

    FilterManager filtermanager;
//
    ostringstream s;
    Indexer indexer(&filtermanager, s);
    indexer.index(argv[argc-1]);
    fprintf(stderr, "%i\n", s.str().length());
    int32_t n = 2*s.str().length();

    // load the file to compare with 
    FileInputStream f(argv[2]);
    BZ2InputStream bz2(&f);
    const char* c;
    n = bz2.read(c, n, n);
    if (n != s.str().length()) {
        printf("output length differs %i instead of %i\n", n, s.str().length());
        return -1;
    }

//    return strncmp(s.str().c_str(), c);

    return 0;
}

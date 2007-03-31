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
#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include "jstreamsconfig.h"
#include "fileinputstream.h"
#include "bz2inputstream.h"
#include "indexer.h"
#include "analyzerconfiguration.h"

#include <cstdio>
#include <cstring>
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_DIRECT_H
 #include <direct.h>
#endif

#include <sstream>
using namespace Strigi;
using namespace std;

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s workingdir dir-to-index referenceoutputfile\n", argv[0]);
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
    if (containsHelp(argc, argv) || (argc != 4)) {
        printUsage(argv);
        return -1;
    }

    const char* mappingfile = 0;

    ostringstream s;
    Strigi::AnalyzerConfiguration ic;
    Indexer indexer(s, ic, mappingfile);
    chdir(argv[1]);
    indexer.index(argv[2]);
    string str = s.str();
    int32_t n = 2*str.length();

    // load the file to compare with
    FileInputStream f(argv[3]);
    BZ2InputStream bz2(&f);
    const char* c;
    n = bz2.read(c, n, n);
    if (n < 0) {
        fprintf(stderr, "Error: %s\n", bz2.error());
        return -1;
    }
    if (n != (int32_t)s.str().length()) {
        printf("output length differs %i instead of %i\n", n, s.str().length());
    }

    const char* p1 = c;
    const char* p2 = str.c_str();
    int32_t n1 = n;
    int32_t n2 = str.length();
    while (n1-- && n2-- && *p1 == *p2) {
        p1++;
        p2++;
    }
    if (*p1 || *p2) {
         printf("difference at position %i\n", p1-c);

         int32_t m = (80 > str.length())?str.length():80;
         printf("%i %.*s\n", m, m, str.c_str());

         m = (80 > n)?n:80;
         printf("%i %.*s\n", m, m, c);

         return -1;
    }

    return 0;
}

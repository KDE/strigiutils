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
#include "dummyindexwriter.h"
#include <strigi/strigiconfig.h>
#include <strigi/diranalyzer.h>
#include <strigi/analyzerconfiguration.h>

#include <stdlib.h>

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s [-v verbosity] [dir-to-index]\n", argv[0]);
}

int
main(int argc, char **argv) {
    if (argc != 2 && argc != 4) {
        printUsage(argv);
        return -1;
    }
    int verbosity = 0;
    if (argc == 4) {
        if (std::strcmp("-v", argv[1])) {
            printUsage(argv);
            return -1;
        }
        verbosity = atoi(argv[2]);
    }

    DummyIndexManager manager(verbosity);
    Strigi::AnalyzerConfiguration ic;
    Strigi::DirAnalyzer analyzer(manager, ic);
    analyzer.analyzeDir(argv[argc-1]);
    return 0;
}

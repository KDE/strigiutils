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
#include "grepindexwriter.h"
#include "jstreamsconfig.h"
#include "indexer.h"
#include "indexerconfiguration.h"
#include "filtermanager.h"

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s PATTERN [dir-or-file-to-grep]\n", argv[0]);
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
    if (containsHelp(argc, argv) || argc < 2) {
        printUsage(argv);
        return -1;
    }
    FilterManager filtermanager;
    GrepIndexWriter writer(argv[1]);
    jstreams::IndexerConfiguration ic;
    Indexer indexer(&filtermanager, writer, ic);
    if (argc > 2) {
        for (int32_t i=2; i<argc; ++i) {
            indexer.index(argv[i]);
        }
    } else {
        indexer.index(".");
    }    
    return 0;
}

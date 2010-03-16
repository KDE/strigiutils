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
#include "dummyindexwriter.h"
#include <strigi/streamanalyzer.h>
#include <strigi/analyzerconfiguration.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/diranalyzer.h>
using namespace Strigi;
using namespace std;

/**
 * Special indexer that indexes only the filenames.
 **/
class FindIndexerConfiguration : public AnalyzerConfiguration {
public:
    bool useFactory(StreamEndAnalyzerFactory* e) const {
        return e->analyzesSubStreams();
    }
    bool useFactory(StreamThroughAnalyzerFactory*) const {return false;}
    bool indexMore() const {return true;}
    bool addMoreText() const {return false;}
    FieldType indexType(const string& fieldname) const {
        return None;
    }
};

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s [dir-or-file-to-find]\n", argv[0]);
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
main(int argc, char **argv) {
    const char* path = ".";
    if (containsHelp(argc, argv) || argc > 2) {
        printUsage(argv);
        return -1;
    }
    if (argc == 2) {
        path = argv[1];
    }

    DummyIndexManager manager(1);
    FindIndexerConfiguration conf;
    DirAnalyzer analyzer(manager, conf);
    analyzer.analyzeDir(path, 1);
    return 0;
}

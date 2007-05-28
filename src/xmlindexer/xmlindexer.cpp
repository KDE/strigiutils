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
 #include <config.h>
#endif

#include "strigiconfig.h"
#include "xmlindexwriter.h"
#include "analyzerconfiguration.h"
#include "diranalyzer.h"
#include <iostream>
#include <cstring>
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_DIRECT_H
 #include <direct.h>
#endif
#include <stdlib.h>

using namespace std;

int
usage(int /*argc*/, char** argv) {
    fprintf(stderr, "Usage: %s [--mappingfile <mappingfile>] "
        "[dirs-or-files-to-index]\n", argv[0]);
    fprintf(stderr, " -j [threads]  Specify the number of threads to use.\n");
    return -1;
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
    vector<string> dirs;
    int nthreads = 2;
    const char* mappingfile = 0;
    int i = 0;
    while (++i < argc) {
        const char* arg = argv[i];
        if (!strcmp("-h", arg) || !strcmp("--help", arg)) {
            return usage(argc, argv);
        }
        if (!strcmp("-j", arg)) {
            if (++i == argc) {
                return usage(argc, argv);
            }
            char* end;
            nthreads = strtol(argv[i], &end, 10);
            if (end == argv[i] || nthreads < 1) {
                return usage(argc, argv);
            }
        } else if (!strcmp("--mappingfile", arg)) {
            if (++i == argc) {
                return usage(argc, argv);
            }
            mappingfile = argv[i];
        } else {
            dirs.push_back(argv[i]);
        }
    }

    if (dirs.size() == 0) {
        char buf[1024];
        getcwd(buf, 1023);
        dirs.push_back(buf);
    }

    vector<pair<bool,string> >filters;
    filters.push_back(make_pair<bool,string>(false,".*/"));
    filters.push_back(make_pair<bool,string>(false,".*"));
    Strigi::AnalyzerConfiguration ic;
    ic.setFilters(filters);

    const TagMapping mapping(mappingfile);
    cout << "<?xml version='1.0' encoding='UTF-8'?>\n<"
        << mapping.map("metadata");
    map<string, string>::const_iterator k = mapping.namespaces().begin();
    while (k != mapping.namespaces().end()) {
        cout << " xmlns:" << k->first << "='" << k->second << "'";
        k++;
    }
    cout << ">\n";

    XmlIndexManager manager(cout, mapping);
    Strigi::DirAnalyzer analyzer(manager, ic);
    for (unsigned i = 0; i < dirs.size(); ++i) {
        analyzer.analyzeDir(dirs[i], nthreads);
    }
    cout << "</" << mapping.map("metadata") << ">\n";

    return 0;
}

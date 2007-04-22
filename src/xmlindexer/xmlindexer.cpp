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

#include "strigiconfig.h"
#include "xmlindexwriter.h"
#include "analyzerconfiguration.h"
#include "diranalyzer.h"
#include <iostream>
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_DIRECT_H
 #include <direct.h>
#endif

using namespace std;

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s [--mappingfile <mappingfile>] "
        "[dirs-or-files-to-index]\n", argv[0]);
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
    if (containsHelp(argc, argv) || argc < 2) {
        printUsage(argv);
        return -1;
    }

    const char* mappingfile = 0;
    vector<const char*> toindex;
    for (int i = 1; i < argc; ++i) {
        if (i < argc-1 && strcmp(argv[i], "--mappingfile") == 0) {
            mappingfile = argv[i+1];
            i++;
        } else {
            toindex.push_back(argv[i]);
        }
    }
    if (toindex.size() == 0) {
        char buf[1024];
        getcwd(buf, 1023);
        toindex.push_back(buf);
    }

    vector<pair<bool,string> >filters;
    filters.push_back(make_pair<bool,string>(false,".*/"));
    filters.push_back(make_pair<bool,string>(false,".*"));
    Strigi::AnalyzerConfiguration ic;
    ic.setFilters(filters);

    const TagMapping mapping(mappingfile);
    ostringstream out;
    out << "<?xml version='1.0' encoding='UTF-8'?>\n<"
        << mapping.map("metadata");
    map<string, string>::const_iterator i = mapping.namespaces().begin();
    while (i != mapping.namespaces().end()) {
        out << " xmlns:" << i->first << "='" << i->second << "'";
        i++;
    }
    out << ">\n";

    XmlIndexWriter writer(cout, mapping);
    Strigi::DirAnalyzer analyzer(writer, &ic);
    for (unsigned i = 0; i < toindex.size(); ++i) {
        analyzer.analyzeDir(toindex[i]);
    }
    out << "</" << mapping.map("metadata") << ">\n";

    return 0;
}

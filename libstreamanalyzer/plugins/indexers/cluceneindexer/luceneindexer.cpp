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
#include <strigi/diranalyzer.h>
#include <strigi/indexpluginloader.h>
#include <strigi/analyzerconfiguration.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <CLucene.h>
#ifdef _MSC_VER
# include <stgdirent.h>
#else
# include <CLucene/util/dirent.h>
#endif

using namespace std;

bool
containsHelp(int argc, char **argv) {
    for (int i=1; i<argc; ++i) {
         if (strcmp(argv[i], "--help") == 0
             || strcmp(argv[i], "-h") == 0) return true;
    }
    return false;
}
bool
checkIndexdirIsEmpty(const char* dir) {
    DIR* d = opendir(dir);
    if (!d) return false;
    struct dirent* de = readdir(d);
    while (de) {
        if (strcmp(de->d_name, "..") && strcmp(de->d_name, ".")) {
            fprintf(stderr, "Directory %s is not empty.\n", dir);
	    closedir(d);
            return false;
        }
        de = readdir(d);
    }
    closedir(d);
    return true;
}
int
main(int argc, char **argv) {
    if (containsHelp(argc, argv) || argc != 3) {
        fprintf(stderr, "Usage: %s [indexdir] [dir-to-index]\n", argv[0]);
        return -1;
    }

    if (!checkIndexdirIsEmpty(argv[1])) {
        return 1;
    }

    vector<pair<bool,string> >filters;
    filters.push_back(make_pair<bool,string>(false,".*/"));
    filters.push_back(make_pair<bool,string>(false,".*"));
    Strigi::AnalyzerConfiguration ic;
    ic.setFilters(filters);
    try {
        Strigi::IndexManager *manager
            = Strigi::IndexPluginLoader::createIndexManager("clucene", argv[1]);
        if (manager) {
            Strigi::DirAnalyzer analyzer(*manager, ic);
            analyzer.analyzeDir(argv[2]);
            Strigi::IndexPluginLoader::deleteIndexManager(manager);
        }
    } catch (...) {
        cerr << "error while creating index" << endl;
    }
    return 0;
}

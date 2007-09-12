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
#include <stdio.h>

#include <strigi/strigiconfig.h>
#include "filelister.h"
#include "analyzerconfiguration.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

/**
 * This test file can be used to measure the performance of the filelister
 * class. The speed can be compared to e.g. "find [path] -printf ''".
 **/

int
main(int argc, char** argv) {
    if (argc != 2) {
        return -1;
    }

//  TODO add the rules to the indexerconfiguration


    Strigi::AnalyzerConfiguration ic;
    Strigi::FileLister lister(&ic);

    lister.startListing(argv[1]);
    string path;
    time_t mtime;
    vector<string> files;
    vector<string> dirs;
    int r = lister.nextFile(path, mtime);
    while (r >= 0) {
        if (r > 0) {
            files.push_back(path);
        } else {
            dirs.push_back(path);
        }
        r = lister.nextFile(path, mtime);
    }

    printf ("files:\n");
    for (unsigned int i = 0; i < files.size(); i++)
        cout <<"\t|"<< files[i] << "|\n";

    printf ("dirs:\n");
    for (unsigned int i = 0; i < dirs.size(); i++)
        cout <<"\t|"<< dirs[i] << "|\n";

    return 0;
}

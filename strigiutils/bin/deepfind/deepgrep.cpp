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
#include "grepindexmanager.h"
#include <strigi/diranalyzer.h>
#include <strigi/analyzerconfiguration.h>
#include <iostream>
#include <cstring>
using namespace Strigi;
using namespace std;

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s [--fields] [--help] PATTERN [dir-or-file-to-grep]\n"
        " --fields print the list of fields\n"
        " --help   print this help screen\n",
        argv[0]);
}
bool
containsArgument(int argc, char **argv, const char* arg, const char* a=0) {
    for (int i=1; i<argc; ++i) {
         if (strcmp(argv[i], arg) == 0
             || (a && strcmp(argv[i], a) == 0)) return true;
    }
    return false;
}
bool
containsHelp(int argc, char **argv) {
    return containsArgument(argc, argv, "--help", "-h");
}
bool
containsFieldList(int argc, char **argv) {
    return containsArgument(argc, argv, "--fields", "-f");
}

void
printFields(AnalyzerConfiguration& conf) {
    const map<string, RegisteredField*>& fields
        = conf.fieldRegister().fields();
    map<string, RegisteredField*>::const_iterator i;
    for (i = fields.begin(); i != fields.end(); ++i) {
        cout << i->first << endl;
    }
}

int
main(int argc, char** argv) {
    AnalyzerConfiguration ic;
    if (containsFieldList(argc, argv)) {
        printFields(ic);
        return 0;
    }
    if (containsHelp(argc, argv) || argc < 2) {
        printUsage(argv);
        return -1;
    }
    GrepIndexManager manager(argv[1]);

    DirAnalyzer analyzer(manager, ic);
    int nthreads = 8;
    if (argc > 2) {
        for (int32_t i=2; i<argc; ++i) {
            analyzer.analyzeDir(argv[i], nthreads);
        }
    } else {
        analyzer.analyzeDir(".", nthreads);
    }
    return 0;
}

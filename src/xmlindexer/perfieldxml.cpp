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
#include "fileinputstream.h"
#include "bz2inputstream.h"
#include "diranalyzer.h"
#include "analyzerconfiguration.h"
#include "xmlindexwriter.h"
#include "streamendanalyzer.h"
#include "streamthroughanalyzer.h"
#include "streamlineanalyzer.h"
#include "streamsaxanalyzer.h"
#include "streameventanalyzer.h"

#include <cstdio>
#include <cstring>
#include <algorithm>
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_DIRECT_H
 #include <direct.h>
#endif

#include <sstream>
#include <iostream>
#include <fstream>
#include <set>
using namespace Strigi;
using namespace std;

/**
 * This class has not been finished yet. It requires changes in all
   analyzer factories.
 **/
class SelectedFieldConfiguration : public Strigi::AnalyzerConfiguration {
public:
    const set<string> requiredFields;
    mutable set<string> usedFields;
    mutable set<string> availableFields;

    explicit SelectedFieldConfiguration(const set<string> af)
        : requiredFields(af) {}

    bool valid() const {
        return requiredFields.size() == usedFields.size();
    }
    bool useFactory(const string& name) const {
        bool use = requiredFields.find(name) != requiredFields.end();
        if (use) {
            usedFields.insert(name);
        }
        availableFields.insert(name);
        return use;
    }
    bool useFactory(StreamEndAnalyzerFactory* f) const {
        return useFactory(f->name());
    }
    bool useFactory(StreamThroughAnalyzerFactory* f) const {
        return useFactory(f->name());
    }
    bool useFactory(StreamSaxAnalyzerFactory* f) const {
        return useFactory(f->name());
    }
    bool useFactory(StreamEventAnalyzerFactory* f) const {
        return useFactory(f->name());
    }
    bool useFactory(StreamLineAnalyzerFactory* f) const {
        return useFactory(f->name());
    }
};

void
printUsage(char** argv) {
    fprintf(stderr, "Usage: %s analyzer file-to-analyze referenceoutputfile\n",
        argv[0]);
}
bool
containsHelp(int argc, char **argv) {
    for (int i=1; i<argc; ++i) {
         if (strcmp(argv[i], "--help") == 0
             || strcmp(argv[i], "-h") == 0) return true;
    }
    return false;
}
set<string>
parseAnalyzerNames(const char* names) {
    set<string> n;
    string ns(names);
    string::size_type start = 0, p = ns.find(',');
    while (p != string::npos) {
        n.insert(ns.substr(start, p-start));
        start  = p + 1;
        p = ns.find(',', start);
    }
    n.insert(ns.substr(start));
    return n;
}
/**
 * Usage: $0 [OPTIONS] SOURCE
 **/
int
main(int argc, char** argv) {
    // there are 2 optional options that both require an argument.
    // one can specify 1 source, so the number of arguments must be
    // 2, 4 or 6
    if (containsHelp(argc, argv) || (argc != 2 && argc != 4 && argc != 6)) {
        printUsage(argv);
        return -1;
    }

    set<string> analyzers;
    const char* targetFile;
    const char* referenceFile = 0;
    if (argc == 4) {
        if (strcmp(argv[1],"-a") == 0) {
            analyzers = parseAnalyzerNames(argv[2]);
        } else if (strcmp(argv[1], "-r") == 0) {
            referenceFile = argv[2];
        } else {
            printUsage(argv);
            return -1;
        }
        targetFile = argv[3];
    } else if (argc == 6) {
        if (strcmp(argv[1], "-a") == 0) {
            analyzers = parseAnalyzerNames(argv[2]);
            if (strcmp(argv[3], "-r") == 0) {
                referenceFile = argv[4];
            }
        } else if (strcmp(argv[1], "-r") == 0) {
            referenceFile = argv[2];
            if (strcmp(argv[3], "-a") == 0) {
                analyzers = parseAnalyzerNames(argv[4]);
            }
        } else {
            printUsage(argv);
            return -1;
        }
        targetFile = argv[5];
    } else {
        targetFile = argv[1];
    }

    const char* mappingFile = 0;

    // check that the target file exists
    {
        ifstream filetest(targetFile);
        if (!filetest.good()) {
            cerr << "The file '" << targetFile << "' cannot be read." << endl;
            return 1;
        }
    }

    const TagMapping mapping(mappingFile);
    ostringstream out;
    out << "<?xml version='1.0' encoding='UTF-8'?>\n<"
        << mapping.map("metadata");
    map<string, string>::const_iterator i = mapping.namespaces().begin();
    while (i != mapping.namespaces().end()) {
        out << " xmlns:" << i->first << "='" << i->second << "'";
        i++;
    }
    out << ">\n";

    ostringstream s;
    SelectedFieldConfiguration ic(analyzers);
    XmlIndexManager manager(s, mapping);
    DirAnalyzer analyzer(manager, ic);
    if (!ic.valid()) {
        set<string>::const_iterator i;
        set<string> missing;
        set_difference(analyzers.begin(), analyzers.end(),
            ic.availableFields.begin(), ic.availableFields.end(),
            insert_iterator<set<string> >(missing, missing.begin()));
        if (missing.size() == 1) {
            fprintf(stderr, "No analyzer with name %s was found.\n",
               missing.begin()->c_str());
        } else {
            cerr << "The analyzers";
            for (i = missing.begin(); i != missing.end(); ++i) {
                cerr << ", " << *i; 
            }
            cerr << " were not found." << endl;
        }
        fprintf(stderr, "Choose from:\n");
        for (i = ic.availableFields.begin(); i != ic.availableFields.end(); ++i) {
            cerr << " " << *i << endl;
        }
        return 1;
    }
    chdir(argv[1]);
    analyzer.analyzeDir(targetFile);
    string str = s.str();
    int32_t n = 2*str.length();

    // if no reference file was specified, we output the analysis
    if (referenceFile == 0) {
        cout << str;
        return 0;
    }

    // load the file to compare with
    FileInputStream f(referenceFile);
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
    if (n1 ==0 && (*p1 || *p2)) {
         printf("difference at position %i\n", p1-c);

         int32_t m = (80 > str.length())?str.length():80;
         printf("%i %.*s\n", m, m, str.c_str());

         m = (80 > n)?n:80;
         printf("%i %.*s\n", m, m, c);

         return -1;
    }

    return 0;
}

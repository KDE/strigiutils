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
#include "indexer.h"
#include "analyzerconfiguration.h"
#include "streamendanalyzer.h"
#include "streamthroughanalyzer.h"
#include "streamlineanalyzer.h"
#include "streamsaxanalyzer.h"
#include "streameventanalyzer.h"

#include <cstdio>
#include <cstring>
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_DIRECT_H
 #include <direct.h>
#endif

#include <sstream>
#include <fstream>
#include <set>
using namespace Strigi;
using namespace std;

class SelectedAnalyzerConfiguration : public Strigi::AnalyzerConfiguration {
public:
    const set<string> requiredAnalyzers;
    mutable set<string> usedAnalyzers;
    mutable set<string> availableAnalyzers;

    explicit SelectedAnalyzerConfiguration(const set<string> an)
        : requiredAnalyzers(an) {}

    bool valid() const {
        return requiredAnalyzers.size() == usedAnalyzers.size()
            || requiredAnalyzers.size() == 0;
    }
    bool useFactory(const string& name) const {
        bool use = requiredAnalyzers.find(name) != requiredAnalyzers.end()
            || requiredAnalyzers.size() == 0;
        if (use) {
            usedAnalyzers.insert(name);
        }
        availableAnalyzers.insert(name);
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
    fprintf(stderr, "Usage: %s [OPTIONS] SOURCE\n"
        "Analyze the given file and output the result as XML.\n"
        " -c   configuration file\n"
        " -a   comma-separated list of analyzers\n"
        " -r   reference output, when specified, the reference output is \n"
        "      compared to the given output and the first difference is \n"
        "      reported.\n",
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
set<string>
parseConfig(const char* config) {
    set<string> n;
    ifstream f(config);
    string line;
    while (f.good()) {
        getline(f, line);
        if (strncmp("analyzer=", line.c_str(), 9) == 0) {
            n.insert(line.substr(9));
        }
    }
    
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
        } else if (strcmp(argv[1], "-c") == 0) {
            analyzers = parseConfig(argv[2]);
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
        } else if (strcmp(argv[1], "-c") == 0) {
            analyzers = parseConfig(argv[2]);
            if (strcmp(argv[3], "-r") == 0) {
                referenceFile = argv[4];
            }
        } else if (strcmp(argv[1], "-r") == 0) {
            referenceFile = argv[2];
            if (strcmp(argv[3], "-a") == 0) {
                analyzers = parseAnalyzerNames(argv[4]);
            } else if (strcmp(argv[3], "-c") == 0) {
                analyzers = parseConfig(argv[4]);
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
    // check that the result file is ok
    FileInputStream f(referenceFile);
//    BZ2InputStream bz2(&f);
    if (referenceFile != 0 && f.status() != Ok) {
        cerr << "The file '" << referenceFile << "' cannot be read." << endl;
        return 1;
    }

    ostringstream s;
    SelectedAnalyzerConfiguration ic(analyzers);
    Indexer indexer(s, ic, mappingFile);
    if (!ic.valid()) {
        set<string>::const_iterator i;
        set<string> missing;
        set_difference(analyzers.begin(), analyzers.end(),
            ic.availableAnalyzers.begin(), ic.availableAnalyzers.end(),
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
        for (i = ic.availableAnalyzers.begin(); i != ic.availableAnalyzers.end(); ++i) {
            cerr << " " << *i << endl;
        }
        return 1;
    }

    // change to the directory of the file to analyze
    // this ensures a consistent naming of the file uris, regardless of cwd
    string targetPath(targetFile);
    string::size_type slashpos = targetPath.rfind('/');
    if (slashpos == string::npos) {
         indexer.index(targetFile);
    } else {
         chdir(targetPath.substr(0,slashpos).c_str());
         indexer.index(targetPath.substr(slashpos+1).c_str());
    }
    string str = s.str();
    int32_t n = 2*str.length();

    // if no reference file was specified, we output the analysis
    if (referenceFile == 0) {
        cout << str;
        return 0;
    }

    // load the file to compare with
    const char* c;
    n = f.read(c, n, n);
    if (n < 0) {
        fprintf(stderr, "Error: %s\n", f.error());
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

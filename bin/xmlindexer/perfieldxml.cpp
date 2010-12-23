/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006,2008 Jos van den Oever <jos@vandenoever.info>
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

#include <strigi/strigiconfig.h>
#include <strigi/fileinputstream.h>
#include <strigi/bz2inputstream.h>
#include <strigi/diranalyzer.h>
#include <strigi/analyzerconfiguration.h>
#include "xmlindexwriter.h"
#include <strigi/streamendanalyzer.h>
#include <strigi/streamthroughanalyzer.h>
#include <strigi/streamlineanalyzer.h>
#include <strigi/streamsaxanalyzer.h>
#include <strigi/streameventanalyzer.h>

#include <cstdio>
#include <cstring>
#include <cerrno>
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
 * Configure analysis bases on what fields we want to extract.
 **/
class SelectedFieldConfiguration : public Strigi::AnalyzerConfiguration {
public:
    /**
     * Fields the user has requested to be reported.
     **/
    const set<string> requiredFields;
    /**
     * Fields that were requested by the user and are provided by some analyzer.
     **/
    mutable set<string> usedFields;
    /**
     * All fields provided by all analyzers.
     **/
    mutable set<string> availableFields;

    explicit SelectedFieldConfiguration(const set<string> af)
        : requiredFields(af) {}

    /**
     * The configuration is valid if all fields requested can be supplied by
     * the active set of analyzer.
     **/
    bool valid() const {
        return requiredFields.size() == usedFields.size();
    }
    /**
     * If a certain field should be reported, return Stored, otherwise return
     * None.
     **/
    FieldType indexType(const Strigi::RegisteredField* f) const {
        return (requiredFields.find(f->key()) != requiredFields.end())
            ? Stored :None;
    }
    /**
     * If any of the fields provided by the given analyzer are requested, use
     * that analyzer for the analysis.
     **/
    bool useAnalyzerFactory(const StreamAnalyzerFactory* f) const {
        bool use = false;
        vector<const RegisteredField*>::const_iterator i;
        i = f->registeredFields().begin();
        const vector<const RegisteredField*>::const_iterator end =
            f->registeredFields().end();
        for (; i != end; ++i) {
            string key((*i)->key());
            availableFields.insert(key);
            bool usethis = requiredFields.find(key) != requiredFields.end();
            if (usethis) {
                use = true;
                usedFields.insert((*i)->key());
            }
        }
        return use;
    }
    bool useFactory(StreamEndAnalyzerFactory* f) const {
        return useAnalyzerFactory(f);
    }
    bool useFactory(StreamThroughAnalyzerFactory* f) const {
        return useAnalyzerFactory(f);
    }
    bool useFactory(StreamSaxAnalyzerFactory* f) const {
        return useAnalyzerFactory(f);
    }
    bool useFactory(StreamEventAnalyzerFactory* f) const {
        return useAnalyzerFactory(f);
    }
    bool useFactory(StreamLineAnalyzerFactory* f) const {
        return useAnalyzerFactory(f);
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
parseFieldNames(const char* names) {
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
        if (strcmp(argv[1],"-f") == 0) {
            analyzers = parseFieldNames(argv[2]);
        } else if (strcmp(argv[1], "-r") == 0) {
            referenceFile = argv[2];
        } else {
            printUsage(argv);
            return -1;
        }
        targetFile = argv[3];
    } else if (argc == 6) {
        if (strcmp(argv[1], "-f") == 0) {
            analyzers = parseFieldNames(argv[2]);
            if (strcmp(argv[3], "-r") == 0) {
                referenceFile = argv[4];
            }
        } else if (strcmp(argv[1], "-r") == 0) {
            referenceFile = argv[2];
            if (strcmp(argv[3], "-f") == 0) {
                analyzers = parseFieldNames(argv[4]);
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
            fprintf(stderr, "No field with name %s was found.\n",
               missing.begin()->c_str());
        } else {
            cerr << "The fields ";
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
    if (chdir(argv[1]) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    analyzer.analyzeDir(targetFile);
    string str = s.str();
    int32_t n = 2*(int32_t)str.length();

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
        cerr << "output length differs " << n << " instead of "
            << s.str().length() << endl;
    }

    const char* p1 = c;
    const char* p2 = str.c_str();
    int32_t n1 = n;
    string::size_type n2 = str.length();
    while (n1-- && n2-- && *p1 == *p2) {
        p1++;
        p2++;
    }
    if (n1 ==0 && (*p1 || *p2)) {
         cerr << "difference at position " << p1-c << endl;

         int32_t m = (80 > str.length())?(int32_t)str.length():80;
         printf("%i %.*s\n", m, m, str.c_str());

         m = (80 > n)?n:80;
         printf("%i %.*s\n", m, m, c);

         return -1;
    }

    return 0;
}

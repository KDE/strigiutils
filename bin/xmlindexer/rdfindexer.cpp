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

#include <strigi/strigiconfig.h>
#include "rdfindexwriter.h"
#include <strigi/analyzerconfiguration.h>
#include <strigi/diranalyzer.h>
#include <strigi/fileinputstream.h>
#include <iostream>
#include <cstring>
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_DIRECT_H
 #include <direct.h>
#endif
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace Strigi;

int
usage(int /*argc*/, char** argv) {
    fprintf(stderr, "Usage: %s\n    [--mappingfile <mappingfile>]\n"
        "    [--lastfiletoskip FILE]\n"
        "    [--stdinmtime mtime]\n    [--stdinfilename filename]\n"
        "    [dirs-or-files-to-index]\n"
        "    [-j nthreads]\n",
        argv[0]);
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
void
analyzeFromStdin(RdfIndexManager& manager, AnalyzerConfiguration& ac,
        const string& filename, time_t mtime) {
    StreamAnalyzer sa(ac);
    sa.setIndexWriter(*manager.indexWriter());
    FileInputStream in(stdin, filename.c_str());
    AnalysisResult result(filename, mtime, *manager.indexWriter(), sa);
    sa.analyze(result, &in);
}

int
main(int argc, char **argv) {
    vector<string> dirs;
    int nthreads = 2;
    const char* mappingfile = 0;
    string lastFileToSkip;
    time_t stdinMTime = time(0);
    string stdinFilename = "-";
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
            nthreads = (int)strtol(argv[i], &end, 10);
            if (end == argv[i] || nthreads < 1) {
                return usage(argc, argv);
            }
        } else if (!strcmp("--mappingfile", arg)) {
            if (++i == argc) {
                return usage(argc, argv);
            }
            mappingfile = argv[i];
        } else if (!strcmp("--lastfiletoskip", arg)) {
            if (++i == argc) {
                return usage(argc, argv);
            }
            lastFileToSkip = argv[i];
        } else if (!strcmp("--stdinmtime", arg)) {
            if (++i == argc) {
                return usage(argc, argv);
            }
            char* end;
            stdinMTime = strtol(argv[i], &end, 10);
            if (end == argv[i] || stdinMTime < 1) {
                return usage(argc, argv);
            }
        } else if (!strcmp("--stdinfilename", arg)) {
            if (++i == argc) {
                return usage(argc, argv);
            }
            stdinFilename = argv[i];
        } else {
            const char* dir = argv[i];
            // remove trailing '/'
            size_t len = strlen(dir);
            if (dir[len-1] == '/') {
                dirs.push_back(std::string(dir, len-1));
            } else {
                dirs.push_back(dir);
            }
        }
    }

    if (dirs.size() == 0) {
        char buf[1024];
        if (getcwd(buf, 1023) == NULL) {
            return -1;
        }
        dirs.push_back(buf);
    }

    vector<pair<bool,string> >filters;
    filters.push_back(make_pair(false,".*/"));
    filters.push_back(make_pair(false,".*"));
    AnalyzerConfiguration ic;
    ic.setFilters(filters);

    const TagMapping mapping(mappingfile);
/*    cout << "<?xml version='1.0' encoding='UTF-8'?>\n<"
        << mapping.map("metadata");
    map<string, string>::const_iterator k = mapping.namespaces().begin();
    while (k != mapping.namespaces().end()) {
        cout << " xmlns:" << k->first << "='" << k->second << "'";
        k++;
    }
    cout << ">\n";
*/
    rdfset rdf;
    
    RdfIndexManager manager(cout, mapping, rdf);
    DirAnalyzer analyzer(manager, ic);
    for (unsigned i = 0; i < dirs.size(); ++i) {
        if (dirs[i] == "-") {
            analyzeFromStdin(manager, ic, stdinFilename, stdinMTime);
        } else {
            analyzer.analyzeDir(dirs[i], nthreads, 0, lastFileToSkip);
        }
    }
//    cout << "</" << mapping.map("metadata") << ">\n";


    for(rdfset::const_iterator subj = rdf.begin(); subj != rdf.end(); subj++) {
      cout<< "<" << subj->first << ">";
      
      std::map<std::string, std::list<std::string> >::const_iterator pred = subj->second.begin();
      do {
        cout << "\n\t<" << pred->first << "> ";
	
	std::list<std::string>::const_iterator obj = pred->second.begin();
	do {	  
	  cout << "\n\t\t\"" << *obj << "\"";
	  obj++;
	  if(obj != pred->second.end())
	    cout <<",";
	} while (obj != pred->second.end());
	pred++;
	if(pred!=subj->second.end())
	  cout << ";";
      } while(pred!=subj->second.end());
      cout<< ".\n";
    }
    
    
    return 0;
}

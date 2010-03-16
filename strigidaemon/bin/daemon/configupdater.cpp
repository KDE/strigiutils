/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Flavio Castelli <flavio.castelli@gmail.com>
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

#include <algorithm>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <set>
#include <string>

#include "daemonconfigurator.h"
#include "filters.h"
#include "strigilogging.h"

using namespace std;

/**
* Check if log4cxx file exists, otherwise creates a default one
**/
void
checkLogConf(const string& filename) {
    std::fstream confFile;
    confFile.open(filename.c_str(), std::ios::in);
    if (!confFile.is_open()){
        /*create the default configuration file*/
        confFile.open(filename.c_str(), std::ios::out);
        confFile << "# Set root logger level to DEBUG and its only appender to A1.\n";
        confFile << "log4j.rootLogger=DEBUG, A1\n\n";
        confFile << "# A1 is set to be a ConsoleAppender.\n";
        confFile << "log4j.appender.A1=org.apache.log4j.ConsoleAppender\n";
        confFile << "# A1 uses PatternLayout.\n";
        confFile << "log4j.appender.A1.layout=org.apache.log4j.PatternLayout\n";
        confFile << "log4j.appender.A1.layout.ConversionPattern=%d [%t] %-5p %c - %m%n\n";
    }
    confFile.close();
}

void loadConfFile(string file, set<string>& entries, string name)
{
    fstream confFile;
    string line;
    char buffer [500];
    unsigned int counter = 0;

    confFile.open(file.c_str(), ios::in);

    if (confFile.is_open())
    {
        // read filter rules
        while (!confFile.eof())
        {
            confFile.getline(buffer, 500);
            line = buffer;

            if (line.size() > 0)
            {
                set<string>::iterator iter;
                iter = find(entries.begin(), entries.end(), line);

                if (iter == entries.end())
                {
                    // it's a new entry
                    counter++;
                    entries.insert (line);
                }
            }
        }

        confFile.close();
    }

    printf ("added %i new %s\n",counter, name.c_str());
}

void help()
{
    printf ("Usage: strigiconfupdaterman [options]\n");
    printf ("\t--merge-filters\tmerge old filters with new ones\n");
    printf ("\t--merge-path\tmerge old paths with new ones\n");
    printf ("\t-s\t\tsimulate, print to stdout\n");
    printf ("\t-o filename\tsave new conf to filename\n");
}

int main(int argc,char *argv[])
{
    if (argc == 1)
    {
        help();
        return 1;
    }

    string homedir = getenv("HOME");
    string daemondir = homedir+"/.strigi";
    string dirsfile = daemondir+"/dirstoindex";
    string pathfilterfile = daemondir+"/pathfilter.conf";
    string patternfilterfile = daemondir+"/patternfilter.conf";
    string conffile = daemondir+"/daemon.conf";
    string logconffile = daemondir+"/log.conf";
    string confout = conffile;
    bool   mergeFilters = false;
    bool   mergePath = false;
    bool   save = true;
    set<string> entries;

    // init logging
    checkLogConf(logconffile);
    STRIGI_LOG_INIT(logconffile);

    while (1)
     {
        int option_index = 0;
        int c;

        static struct option long_options[] = {
            {"merge-filters", 0, 0, 1},
            {"merge-path", 0, 0, 2},
            {"help", 0, 0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "h:o:s",
                 long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 1:
                //merge filters
                mergeFilters = true;
                break;
            case 2:
                //merge path
                mergePath = true;
                break;
            case 'o':
                confout = string(optarg);
                break;
            case 's':
                save = false;
                break;
            case 'h':
                help();
                return 0;
                break;
            default:
                help();
                return 1;
        }
    }

    // init daemon configurator
    DaemonConfigurator config (conffile);

    // load path filtering rules

    if (mergeFilters) // read existing rules, avoid duplicate rules creation
    {
        printf ("merging ");
        entries = config.readFilteringRules();
    }
    else
        printf ("restoring ");
    printf ("path filtering rules\n");

    loadConfFile (pathfilterfile, entries, "path filtering rules");
    config.saveFilteringRules(entries, PathFilter::RTTI, false);

    entries.clear();

    // load pattern filtering rules
    printf ("restoring pattern filtering rules\n");
    loadConfFile (patternfilterfile, entries, "pattern filtering rules");
    config.saveFilteringRules(entries, PatternFilter::RTTI, true);

    entries.clear();

    // load dirs to index
    if (mergePath)
    {
        entries = config.getIndexedDirectories();
        printf ("merging ");
    }
    else
        printf ("restoring ");
    printf ("indexed directories paths\n");

    loadConfFile (dirsfile, entries, "indexed dirs paths");
    config.setIndexedDirectories( entries, "localhost", mergePath);

    if (save)
    {
        printf ("writing new configuration file\n");
        config.save (confout.c_str());
    }
    else
        cout << endl << endl << config;

    return 0;
}

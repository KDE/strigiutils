/* This file is part Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <set>

#include "XesamQLParser.h"
#include "StrigiQueryBuilder.h"
#include "xesam_ul_driver.hh"
#include "strigilogging.h"

using namespace std;
using namespace Dijon;
using namespace Strigi;

int main(int argc, char *argv[])
{
    STRIGI_LOG_INIT_BASIC();
  
    int opt;
    set<string> ulQueries, qlQueries, ulStringQueries;
    string outfile;

    outfile.clear();

    if (argc < 3)
    {
        fprintf(stderr,
                "Usage: %s [-u ulQueryFile] [-U XesamUserLanguageQuery ] "
                "[-q qlQueryFile] [-o outputfile]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    
    while ((opt = getopt(argc, argv, "u:U:q:o:")) != -1)
    {
        switch (opt) {
            case 'u':
                // user language query
                ulQueries.insert (optarg);
                break;
            case 'U':
                // user language query
                ulStringQueries.insert (optarg);
                break;
            case 'q':
                // query language query
                qlQueries.insert (optarg);
                break;
            case 'o':
                outfile = optarg;
                break;
            default: /* ’?’ */
                fprintf(stderr,
                        "Usage: %s [-u ulQueryFile] [-q qlQueryFile]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    for (set<string>::iterator iter = qlQueries.begin();
         iter != qlQueries.end(); ++iter)
    {
        printf ("processing query from file %s\n", (*iter).c_str());
        
        ifstream queryfile;

        queryfile.open ((*iter).c_str(), ifstream::in);

        if (!queryfile.is_open())
        {
            fprintf (stderr, "Unable to open file %s\n", (*iter).c_str());
            continue;
        }
        
        string querytext;

        char buffer [500];
        
        while (!queryfile.eof())
        {
            queryfile.getline (buffer, 500);
            querytext += buffer;
        }

        queryfile.close();
        
        XesamQLParser xesamQlParser;
        StrigiQueryBuilder strigiQueryBuilder;

        if (xesamQlParser.parse ( querytext, strigiQueryBuilder))
        {
            printf ("query parsed successfully\n");
            Query query = strigiQueryBuilder.get_query();
            if (outfile.length() == 0)
                continue;

            ofstream out;
            out.open (outfile.c_str());
            out << query;
            out.close();
        }
        else
            printf ("error parsing query\n");
    }

    
    for (set<string>::iterator iter = ulQueries.begin();
         iter != ulQueries.end(); ++iter)
    {
        printf ("processing user language query from file %s\n",
                (*iter).c_str());

        XesamUlDriver driver;
        if (driver.parseFile (*iter)) {
          Query* query = driver.query();

          if (!outfile.empty()) {
              ofstream out;

              out.open (outfile.c_str());
              if (out.is_open()) {
                  out << *query;
                  out.close();
              }
              else
                cerr << "unable to write to file " << outfile << endl;
          }
        }
        else
          cerr << "unable to parse xesam query, check syntax" << endl;
    }

    for (set<string>::iterator iter = ulStringQueries.begin();
         iter != ulStringQueries.end(); ++iter)
    {
        printf ("processing user language query specified by command "
                "line |%s|\n", (*iter).c_str());

        XesamUlDriver driver;
        driver.parseString( *iter);
        Query* query = driver.query();

        if (!outfile.empty()) {
            ofstream out;

            out.open (outfile.c_str());
            if (out.is_open()) {
                out << *query;
                out.close();
            }
            else
                cerr << "unable to write to file " << outfile << endl;
        }
    }
    
    return 0;
}

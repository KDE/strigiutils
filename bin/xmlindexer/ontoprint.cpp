/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <list>
#include <algorithm>
#include <unistd.h>
#include <getopt.h>
#include <strigi/fieldpropertiesdb.h>
#include <strigi/streamanalyzer.h>
#include <strigi/analyzerconfiguration.h>
using namespace std;
using namespace Strigi;

void
printDot(ostream& out, const char* locale) {
    const map<string, FieldProperties>& p
        = FieldPropertiesDb::db().allProperties();
    map<string, FieldProperties>::const_iterator i;
    list<string> categories;
    out << "digraph{graph[rankdir=LR];" << endl;
    for (i = p.begin(); i != p.end(); ++i) {
        const vector<string>& parents = i->second.parentUris();
        vector<string>::const_iterator j;
        out << '"' << i->second.uri() << "\" [shape=record, label =\"" << i->second.uri() << "|{type: " << i->second.typeUri() << "}\"];" << endl;
        for (j = parents.begin(); j != parents.end(); ++j) {
            out << '"' << *j << "\"->\"" << i->second.uri() << "\";" << endl;
        }
        // make link to category, e.g. chemistry for chemistry.inchi
        string category = i->second.uri().substr(0,i->second.uri().find("."));
        if (category.length() != i->second.uri().length()) {
            list<string>::const_iterator match = find(categories.begin(), categories.end(), category);
            if (match == categories.end()) {
                categories.push_back(category);
                out << "\"" << category << "\" [style=filled,color=gray];" << endl;
            }
            out << "\"" << category << "\"->\"" << i->second.uri() << "\";" << endl;
        }
    }
    out << "}" << endl;
}
void
printRdfsProperties(ostream& out, const FieldProperties& p) {
    out << " <rdf:Property rdf:about='" << p.uri() << "'>\n"
        << "  <rdfs:label>" << p.name() << "</rdfs:label>\n"
        << "  <rdfs:comment>" << p.description() << "</rdfs:comment>\n";
    const vector<string>& parents = p.parentUris();
    vector<string>::const_iterator j;
    for (j = parents.begin(); j != parents.end(); ++j){
        out << "  <rdfs:subPropertyOf rdf:resource='" << *j << "'/>\n";
    }
    const vector<string>& classes = p.applicableClasses();
    for (j = classes.begin(); j != classes.end(); ++j){
        out << "  <rdfs:domain rdf:resource='" << *j << "'/>\n";
    }

    out << "  <rdfs:range rdf:resource='" << p.typeUri() << "'/>\n";
    const vector<string>& locales = p.locales();
    for (j = locales.begin(); j != locales.end(); ++j) {
        const string& name = p.localizedName(*j);
	if (name.size()) {
            out << "  <rdfs:label xml:lang='" << *j << "'>" << name << "</rdfs:label>\n";
        }
        const string& description = p.localizedDescription(*j);
	if (description.size()) {
            out << "  <rdfs:comment xml:lang='" << *j << "'>" << description << "</rdfs:comment>\n";
        }
    }
    out << " </rdf:Property>\n";
}

void
printRdfsClasses(ostream& out, const ClassProperties& p) {
    out << " <rdfs:Class rdf:about='" << p.uri() << "'>\n"
        << "  <rdfs:label>" << p.name() << "</rdfs:label>\n"
        << "  <rdfs:comment>" << p.description() << "</rdfs:comment>\n";
    const vector<string>& parents = p.parentUris();
    vector<string>::const_iterator j;
    for (j = parents.begin(); j != parents.end(); ++j){
        out << "  <rdfs:subClassOf rdf:resource='" << *j << "'/>\n";
    }

    const vector<string>& locales = p.locales();
    for (j = locales.begin(); j != locales.end(); ++j) {
        const string& name = p.localizedName(*j);
	if (name.size()) {
            out << "  <rdfs:label xml:lang='" << *j << "'>" << name << "</rdfs:label>\n";
        }
        const string& description = p.localizedDescription(*j);
	if (description.size()) {
            out << "  <rdfs:comment xml:lang='" << *j << "'>" << description << "</rdfs:comment>\n";
        }
    }
    out << " </rdfs:Class>\n";
}

void
printRdfs(ostream& out) {
    out << "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<!DOCTYPE rdf:RDF [\n"
        "    <!ENTITY rdf 'http://www.w3.org/1999/02/22-rdf-syntax-ns#'>\n"
        "    <!ENTITY strigi 'http://strigi URL goes here#'>\n"
        "    <!ENTITY rdfs 'http://www.w3.org/2000/01/rdf-schema#'>\n"
        "]>\n"
        "<rdf:RDF "
            "xmlns:rdf='&rdf;' xmlns:strigi='&strigi;' xmlns:rdfs='&rdfs;'>\n";

    const map<string, FieldProperties>& p
        = FieldPropertiesDb::db().allProperties();
    map<string, FieldProperties>::const_iterator i;
    for (i = p.begin(); i != p.end(); ++i) {
        printRdfsProperties(out, i->second);
    }

    const map<string, ClassProperties>& c
        = FieldPropertiesDb::db().allClasses();
    map<string, ClassProperties>::const_iterator j;
    for (j = c.begin(); j != c.end(); ++j) {
        printRdfsClasses(out, j->second);
    }

    out << "</rdf:RDF>" << endl;
}
void
printHelp(const char* program) {
    cerr << "Usage: " << program << " [--type=<type>] [--locale=<locale>]"
        << endl;
}
int
main(int argc, char** argv) {
    struct option long_options[] = {
        {"help",   no_argument,       0, 0},
        {"type",   required_argument, 0, 0},
        {"locale", required_argument, 0, 0}
    };
    const char* type = 0;
    const char* locale = 0;
    bool help = false;
    while (1) {
        int optindex;
        int c = getopt_long(argc, argv, "", long_options, &optindex);
        if (c == -1) break;
        if (c == 0) {
            if (optindex == 0) help = true;
            if (optindex == 1) type   = optarg;
            if (optindex == 2) locale = optarg;
        }
        switch (c) {
        case '?':
            printHelp(argv[0]);
            exit(1);
        default:
            break;
        }
    }

    // load the plugins
    AnalyzerConfiguration ac;
    StreamAnalyzer s(ac);

    if (help) {
        printHelp(argv[0]); 
    } else if (type && strcmp(type, "dot") == 0) {
        printDot(cout, locale);
    } else {
        printRdfs(cout);
    }
    return 0;
}

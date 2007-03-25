#include <iostream>
#include "fieldpropertiesdb.h"
using namespace std;
using namespace Strigi;

void
printDot(ostream& out) {
    const map<string, FieldProperties>& p
        = FieldPropertiesDb::db().allProperties();
    map<string, FieldProperties>::const_iterator i;
    out << "digraph {" << endl;
    for (i = p.begin(); i != p.end(); ++i) {
        const vector<string>& parents = i->second.parentUris();
        vector<string>::const_iterator j;
        for (j = parents.begin(); j != parents.end(); ++j) {
            out << '"' << *j << "\"->\"" << i->second.uri() << '"' << endl;
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
    for (j = parents.begin(); j != parents.end(); ++j) {
        out << "  <rdfs:subPropertyOf rdf:resource='" << *j << "'/>\n";
    }
    out << "  <rdfs:range rdf:resource='" << p.typeUri() << "'/>\n";
    const map<string, FieldProperties::Localized>& l = p.localized();
    map<string, FieldProperties::Localized>::const_iterator k;
    for (k = l.begin(); k != l.end(); ++k) {
        out << "  <rdfs:label xml:lang='" << k->first << "'>" << k->second.name
            << "</rdfs:label>\n";
        out << "  <rdfs:comment xml:lang='" << k->first << "'>"
            << k->second.description << "</rdfs:comment>\n";
    }
    out << "  <rdfs:domain rdf:resource='&strigi;file'/>\n"
        << " </rdf:Property>\n";
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
    out << "</rdf:RDF>" << endl;
}
void
printHelp(const char* program) {
    cerr << "Usage: " << program << " [--dot] [--rdfs]" << endl;
}

int
main(int argc, const char** argv) {
    if (argc == 2) {
        if (strcmp("--rdfs", argv[1]) == 0) {
            printRdfs(cout);
        } else if (strcmp("--dot", argv[1]) == 0) {
            printDot(cout);
        }
        return 0;
    }
    printHelp(argv[0]); 
    return 1;
}

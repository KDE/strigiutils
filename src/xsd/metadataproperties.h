/* DO NOT EDIT */
/* This file is generated from metadataproperties.xsd */
#ifndef METADATAPROPERTIES_H
#define METADATAPROPERTIES_H
#include "xsdparser/xmlstream.h"
#include <string>
#include <list>
class Metadata {
public:
	Metadata(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Metadata&);
	std::string a_key;
};
class Metadataproperties {
public:
	Metadataproperties(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Metadataproperties&);
	std::list<Metadata> e_metadata;
};
#endif

/* This file is generated from metadataproperties.xsd */
#include <iostream>
#include <fstream>
#include <sstream>
#include "metadataproperties.h"
std::string
read(const std::string& file) {
	std::stringbuf buf;
	std::ifstream f(file.c_str(), std::ios::binary);
	f.get(buf, '\0');
	f.close();
	return buf.str();
}
int
main() {
	std::string xml;
	int n = 1;
	std::ofstream f;

	std::ostringstream filename;

	filename << n++ << ".xml";
	xml = read(filename.str());
	Metadata metadata(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << metadata;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	Metadataproperties metadataproperties(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << metadataproperties;
	f.close();
	filename.str("");


	return 0;
}

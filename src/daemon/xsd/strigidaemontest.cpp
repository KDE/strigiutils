/* This file is generated from strigidaemon.xsd */
#include <iostream>
#include <fstream>
#include <sstream>
#include "strigidaemon.h"
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
	Paths paths(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << paths;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	Pathfilter pathfilter(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << pathfilter;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	Patternfilter patternfilter(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << patternfilter;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	Repository repository(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << repository;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	Strigidaemon strigidaemon(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << strigidaemon;
	f.close();
	filename.str("");


	return 0;
}

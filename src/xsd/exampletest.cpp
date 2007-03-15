/* This file is generated from example.xsd */
#include <iostream>
#include <fstream>
#include <sstream>
#include "example.h"
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
	GeneralInfo generalinfo(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << generalinfo;
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
	DaemonConfiguration daemonconfiguration(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << daemonconfiguration;
	f.close();
	filename.str("");


	return 0;
}

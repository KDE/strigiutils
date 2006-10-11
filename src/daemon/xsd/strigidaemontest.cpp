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
	DaemonConfiguration daemonconfiguration(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << daemonconfiguration;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	DaemonConfigurationType daemonconfigurationtype(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << daemonconfigurationtype;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	RepositoryType repositorytype(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << repositorytype;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	RepositoryTypeType repositorytypetype(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << repositorytypetype;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	FileSourceType filesourcetype(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << filesourcetype;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	FileSystemSourceType filesystemsourcetype(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << filesystemsourcetype;
	f.close();
	filename.str("");

	filename << n++ << ".xml";
	xml = read(filename.str());
	HttpSourceType httpsourcetype(xml);
	f.open(filename.str().c_str(), std::ios::binary);
	f << httpsourcetype;
	f.close();
	filename.str("");


	return 0;
}

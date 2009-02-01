/* DO NOT EDIT */
/* This file is generated from example.xsd */
#ifndef EXAMPLE_H
#define EXAMPLE_H
#include "xsdparser/xmlstream.h"
#include <string>
#include <list>
class GeneralInfo {
public:
	GeneralInfo(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const GeneralInfo&);
	std::string a_name;
};
class Repository {
public:
	Repository(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Repository&);
	std::string a_repositoryType;
};
class DaemonConfiguration {
public:
	DaemonConfiguration(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const DaemonConfiguration&);
	std::string a_repositoryLocation;
	std::string a_repositoryType;
	GeneralInfo e_generalinfo;
	std::list<Repository> e_repository;
	std::string e_info;
};
#endif

/* This file is generated from example.xsd */
#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <string>
#include <list>
class GeneralInfo {
public:
	GeneralInfo(const std::string& xml = "");
	std::string a_name;
};
std::ostream& operator<<(std::ostream&, const GeneralInfo&);
class Repository {
public:
	Repository(const std::string& xml = "");
	std::string a_repositoryType;
};
std::ostream& operator<<(std::ostream&, const Repository&);
class DaemonConfiguration {
public:
	DaemonConfiguration(const std::string& xml = "");
	std::string a_repositoryLocation;
	std::string a_repositoryType;
	GeneralInfo e_generalinfo;
	std::list<Repository> e_repository;
	std::string e_info;
};
std::ostream& operator<<(std::ostream&, const DaemonConfiguration&);
#endif

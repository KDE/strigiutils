/* This file is generated from strigidaemonconfiguration.xsd */
#ifndef STRIGIDAEMONCONFIGURATION_H
#define STRIGIDAEMONCONFIGURATION_H
#include "xsdparser/xmlstream.h"
#include <string>
#include <list>
class Path {
public:
	Path(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Path&);
	std::string a_path;
};
class Repository {
public:
	Repository(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Repository&);
	std::string a_indexdir;
	std::string a_type;
	bool a_writeable;
	std::string a_name;
	std::list<Path> e_path;
};
class Pathfilter {
public:
	Pathfilter(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Pathfilter&);
	std::string a_path;
};
class Patternfilter {
public:
	Patternfilter(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Patternfilter&);
	std::string a_pattern;
};
class Filteringrules {
public:
	Filteringrules(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Filteringrules&);
	std::list<Pathfilter> e_pathfilter;
	std::list<Patternfilter> e_patternfilter;
};
class StrigiDaemonConfiguration {
public:
	StrigiDaemonConfiguration(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const StrigiDaemonConfiguration&);
	friend XMLStream& operator>>(XMLStream& in, StrigiDaemonConfiguration&);
protected:
	bool a_useDBus;
	std::list<Repository> e_repository;
	std::list<Filteringrules> e_filteringrules;
};
#endif

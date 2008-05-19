/* DO NOT EDIT */
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
	std::string a_name;
	bool a_writeable;
	int a_pollingInterval;
	std::string a_urlbase;
	std::string a_indexdir;
	std::string a_type;
	std::list<Path> e_path;
};
class Filter {
public:
	Filter(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Filter&);
	std::string a_pattern;
	bool a_include;
};
class Filters {
public:
	Filters(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const Filters&);
	std::list<Filter> e_filter;
};
class StrigiDaemonConfiguration {
public:
	StrigiDaemonConfiguration(const std::string& xml = "");
	friend std::ostream& operator<<(std::ostream&, const StrigiDaemonConfiguration&);
	friend XMLStream& operator>>(XMLStream& in, StrigiDaemonConfiguration&);
protected:
	bool a_useDBus;
	std::list<Repository> e_repository;
	Filters e_filters;
};
#endif

/* This file is generated from strigidaemonconfiguration.xsd */
#ifndef STRIGIDAEMONCONFIGURATION_H
#define STRIGIDAEMONCONFIGURATION_H
#include <string>
#include <list>
class Path {
public:
	Path(const std::string& xml = "");
	std::string a_path;
};
std::ostream& operator<<(std::ostream&, const Path&);
class Pathfilter {
public:
	Pathfilter(const std::string& xml = "");
	std::string a_path;
};
std::ostream& operator<<(std::ostream&, const Pathfilter&);
class Patternfilter {
public:
	Patternfilter(const std::string& xml = "");
	std::string a_pattern;
};
std::ostream& operator<<(std::ostream&, const Patternfilter&);
class Repository {
public:
	Repository(const std::string& xml = "");
	std::string a_type;
	std::list<Path> e_path;
	std::list<Pathfilter> e_pathfilter;
	std::list<Patternfilter> e_patternfilter;
};
std::ostream& operator<<(std::ostream&, const Repository&);
class StrigiDaemonConfiguration {
public:
	StrigiDaemonConfiguration(const std::string& xml = "");
	bool a_useDBus;
	std::list<Repository> e_repository;
};
std::ostream& operator<<(std::ostream&, const StrigiDaemonConfiguration&);
#endif

/* This file is generated from strigidaemon.xsd */
#ifndef STRIGIDAEMON_H
#define STRIGIDAEMON_H
#include <string>
#include <list>
class Paths {
public:
	Paths(const std::string& xml = "");
	std::string a_path;
};
std::ostream& operator<<(std::ostream&, const Paths&);
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
	Paths e_paths;
	Pathfilter e_pathfilter;
	Patternfilter e_patternfilter;
};
std::ostream& operator<<(std::ostream&, const Repository&);
class Strigidaemon {
public:
	Strigidaemon(const std::string& xml = "");
	std::list<Repository> e_repository;
};
std::ostream& operator<<(std::ostream&, const Strigidaemon&);
#endif

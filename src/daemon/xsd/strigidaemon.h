/* This file is generated from strigidaemon.xsd */
#ifndef STRIGIDAEMON_H
#define STRIGIDAEMON_H
#include <string>
#include <list>
class DaemonConfiguration {
public:
	DaemonConfiguration(const std::string& xml = "");
};
std::ostream& operator<<(std::ostream&, const DaemonConfiguration&);
class DaemonConfigurationType {
public:
	DaemonConfigurationType(const std::string& xml = "");
};
std::ostream& operator<<(std::ostream&, const DaemonConfigurationType&);
class RepositoryType {
public:
	RepositoryType(const std::string& xml = "");
};
std::ostream& operator<<(std::ostream&, const RepositoryType&);
class RepositoryTypeType {
public:
	RepositoryTypeType(const std::string& xml = "");
};
std::ostream& operator<<(std::ostream&, const RepositoryTypeType&);
class FileSourceType {
public:
	FileSourceType(const std::string& xml = "");
};
std::ostream& operator<<(std::ostream&, const FileSourceType&);
class FileSystemSourceType {
public:
	FileSystemSourceType(const std::string& xml = "");
};
std::ostream& operator<<(std::ostream&, const FileSystemSourceType&);
class HttpSourceType {
public:
	HttpSourceType(const std::string& xml = "");
};
std::ostream& operator<<(std::ostream&, const HttpSourceType&);
#endif

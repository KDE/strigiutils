/* This file is generated from strigidaemon.xsd */
#include "strigidaemon.h"
#include <iostream>
#include "xmlstream.h"
XMLStream&
operator>>(XMLStream& in, DaemonConfiguration& e) {
	return in;
}
DaemonConfiguration::DaemonConfiguration(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const DaemonConfiguration& e) {
	out << "<daemonConfiguration";
	out << ">\n";
	out << "</daemonConfiguration>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, DaemonConfigurationType& e) {
	return in;
}
DaemonConfigurationType::DaemonConfigurationType(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const DaemonConfigurationType& e) {
	out << "<daemonConfigurationType";
	out << ">\n";
	out << "</daemonConfigurationType>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, RepositoryType& e) {
	return in;
}
RepositoryType::RepositoryType(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const RepositoryType& e) {
	out << "<repositoryType";
	out << ">\n";
	out << "</repositoryType>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, RepositoryTypeType& e) {
	return in;
}
RepositoryTypeType::RepositoryTypeType(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const RepositoryTypeType& e) {
	out << "<repositoryTypeType";
	out << ">\n";
	out << "</repositoryTypeType>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, FileSourceType& e) {
	return in;
}
FileSourceType::FileSourceType(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const FileSourceType& e) {
	out << "<fileSourceType";
	out << ">\n";
	out << "</fileSourceType>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, FileSystemSourceType& e) {
	return in;
}
FileSystemSourceType::FileSystemSourceType(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const FileSystemSourceType& e) {
	out << "<fileSystemSourceType";
	out << ">\n";
	out << "</fileSystemSourceType>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, HttpSourceType& e) {
	return in;
}
HttpSourceType::HttpSourceType(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const HttpSourceType& e) {
	out << "<httpSourceType";
	out << ">\n";
	out << "</httpSourceType>\n";

	return out;
}

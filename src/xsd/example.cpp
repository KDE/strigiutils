/* DO NOT EDIT */
/* This file is generated from example.xsd */
#include "example.h"
#include <iostream>
XMLStream&
operator>>(XMLStream& in, GeneralInfo& e) {
	in.setFromAttribute(e.a_name,"name");
	return in;
}
GeneralInfo::GeneralInfo(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const GeneralInfo& e) {
	out << " <generalInfo";
	out << " name='" << e.a_name << "'";
	out << ">\n";
	out << " </generalInfo>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, Repository& e) {
	in.setFromAttribute(e.a_repositoryType,"repositoryType");
	return in;
}
Repository::Repository(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Repository& e) {
	out << " <repository";
	out << " repositoryType='" << e.a_repositoryType << "'";
	out << ">\n";
	out << " </repository>\n";

	return out;
}
XMLStream& operator>>(XMLStream&, GeneralInfo&);
XMLStream& operator>>(XMLStream&, Repository&);
XMLStream&
operator>>(XMLStream& in, DaemonConfiguration& e) {
	in.setFromAttribute(e.a_repositoryLocation,"repositoryLocation");
	in.setFromAttribute(e.a_repositoryType,"repositoryType");
	const SimpleNode* n = in.firstChild();
	bool hasChildren = n;
	if (n && in.getTagName() == "generalInfo") {
		in >> e.e_generalinfo;
		n = in.nextSibling();
	}
	while (n && in.getTagName() == "repository") {
		Repository v;
		in >> v;
		e.e_repository.push_back(v);
		n = in.nextSibling();
	}
	if (n && in.getTagName() == "info") {
		in >> e.e_info;
		n = in.nextSibling();
	}
	if (hasChildren) {
		in.parentNode();
	}
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
	out << " repositoryLocation='" << e.a_repositoryLocation << "'";
	out << " repositoryType='" << e.a_repositoryType << "'";
	out << ">\n";
	out << e.e_generalinfo;
	std::list<Repository>::const_iterator repository_it;
	for (repository_it = e.e_repository.begin(); repository_it != e.e_repository.end(); repository_it++) {
		out << *repository_it;
	}
	out << e.e_info;
	out << "</daemonConfiguration>\n";

	return out;
}

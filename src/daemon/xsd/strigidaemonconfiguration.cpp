/* This file is generated from strigidaemonconfiguration.xsd */
#include "strigidaemonconfiguration.h"
#include <iostream>
#include "xmlstream.h"
XMLStream&
operator>>(XMLStream& in, Paths& e) {
	in.setFromAttribute(e.a_path,"path");
	return in;
}
Paths::Paths(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Paths& e) {
	out << " <paths";
	out << " path='" << e.a_path << "'";
	out << ">\n";
	out << " </paths>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, Pathfilter& e) {
	in.setFromAttribute(e.a_path,"path");
	return in;
}
Pathfilter::Pathfilter(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Pathfilter& e) {
	out << " <pathfilter";
	out << " path='" << e.a_path << "'";
	out << ">\n";
	out << " </pathfilter>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, Patternfilter& e) {
	in.setFromAttribute(e.a_pattern,"pattern");
	return in;
}
Patternfilter::Patternfilter(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Patternfilter& e) {
	out << " <patternfilter";
	out << " pattern='" << e.a_pattern << "'";
	out << ">\n";
	out << " </patternfilter>\n";

	return out;
}
XMLStream& operator>>(XMLStream&, Paths&);
XMLStream& operator>>(XMLStream&, Pathfilter&);
XMLStream& operator>>(XMLStream&, Patternfilter&);
XMLStream&
operator>>(XMLStream& in, Repository& e) {
	const SimpleNode* n = in.firstChild();
	bool hasChildren = n;
	if (n && in.getTagName() == "paths") {
		in >> e.e_paths;
		n = in.nextSibling();
	}
	if (n && in.getTagName() == "pathfilter") {
		in >> e.e_pathfilter;
		n = in.nextSibling();
	}
	if (n && in.getTagName() == "patternfilter") {
		in >> e.e_patternfilter;
		n = in.nextSibling();
	}
	if (hasChildren) {
		in.parentNode();
	}
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
	out << ">\n";
	out << e.e_paths;
	out << e.e_pathfilter;
	out << e.e_patternfilter;
	out << " </repository>\n";

	return out;
}
XMLStream& operator>>(XMLStream&, Repository&);
XMLStream&
operator>>(XMLStream& in, StrigiDaemonConfiguration& e) {
	const SimpleNode* n = in.firstChild();
	bool hasChildren = n;
	while (n && in.getTagName() == "repository") {
		Repository v;
		in >> v;
		e.e_repository.push_back(v);
		n = in.nextSibling();
	}
	if (hasChildren) {
		in.parentNode();
	}
	return in;
}
StrigiDaemonConfiguration::StrigiDaemonConfiguration(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const StrigiDaemonConfiguration& e) {
	out << "<strigiDaemonConfiguration";
	out << ">\n";
	std::list<Repository>::const_iterator repository_it;
	for (repository_it = e.e_repository.begin(); repository_it != e.e_repository.end(); repository_it++) {
		out << *repository_it;
	}
	out << "</strigiDaemonConfiguration>\n";

	return out;
}

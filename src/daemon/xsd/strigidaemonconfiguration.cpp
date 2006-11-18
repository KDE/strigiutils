/* This file is generated from strigidaemonconfiguration.xsd */
#include "strigidaemonconfiguration.h"
#include <iostream>
XMLStream&
operator>>(XMLStream& in, Path& e) {
	in.setFromAttribute(e.a_path,"path");
	return in;
}
Path::Path(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Path& e) {
	out << " <path";
	out << " path='" << e.a_path << "'";
	out << ">\n";
	out << " </path>\n";

	return out;
}
XMLStream& operator>>(XMLStream&, Path&);
XMLStream&
operator>>(XMLStream& in, Repository& e) {
	in.setFromAttribute(e.a_indexdir,"indexdir");
	in.setFromAttribute(e.a_urlbase,"urlbase");
	in.setFromAttribute(e.a_type,"type");
	in.setFromAttribute(e.a_pollingInterval,"pollingInterval");
	in.setFromAttribute(e.a_writeable,"writeable");
	in.setFromAttribute(e.a_name,"name");
	const SimpleNode* n = in.firstChild();
	bool hasChildren = n;
	while (n && in.getTagName() == "path") {
		Path v;
		in >> v;
		e.e_path.push_back(v);
		n = in.nextSibling();
	}
	if (hasChildren) {
		in.parentNode();
	}
	return in;
}
Repository::Repository(const std::string& xml) {
	a_pollingInterval = 0;
	a_writeable = false;
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Repository& e) {
	out << " <repository";
	out << " indexdir='" << e.a_indexdir << "'";
	out << " urlbase='" << e.a_urlbase << "'";
	out << " type='" << e.a_type << "'";
	out << " pollingInterval='" << e.a_pollingInterval << "'";
	out << " writeable='" << e.a_writeable << "'";
	out << " name='" << e.a_name << "'";
	out << ">\n";
	std::list<Path>::const_iterator path_it;
	for (path_it = e.e_path.begin(); path_it != e.e_path.end(); path_it++) {
		out << *path_it;
	}
	out << " </repository>\n";

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
XMLStream& operator>>(XMLStream&, Pathfilter&);
XMLStream& operator>>(XMLStream&, Patternfilter&);
XMLStream&
operator>>(XMLStream& in, Filteringrules& e) {
	const SimpleNode* n = in.firstChild();
	bool hasChildren = n;
	while (n && in.getTagName() == "pathfilter") {
		Pathfilter v;
		in >> v;
		e.e_pathfilter.push_back(v);
		n = in.nextSibling();
	}
	while (n && in.getTagName() == "patternfilter") {
		Patternfilter v;
		in >> v;
		e.e_patternfilter.push_back(v);
		n = in.nextSibling();
	}
	if (hasChildren) {
		in.parentNode();
	}
	return in;
}
Filteringrules::Filteringrules(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Filteringrules& e) {
	out << " <filteringrules";
	out << ">\n";
	std::list<Pathfilter>::const_iterator pathfilter_it;
	for (pathfilter_it = e.e_pathfilter.begin(); pathfilter_it != e.e_pathfilter.end(); pathfilter_it++) {
		out << *pathfilter_it;
	}
	std::list<Patternfilter>::const_iterator patternfilter_it;
	for (patternfilter_it = e.e_patternfilter.begin(); patternfilter_it != e.e_patternfilter.end(); patternfilter_it++) {
		out << *patternfilter_it;
	}
	out << " </filteringrules>\n";

	return out;
}
XMLStream& operator>>(XMLStream&, Repository&);
XMLStream& operator>>(XMLStream&, Filteringrules&);
XMLStream&
operator>>(XMLStream& in, StrigiDaemonConfiguration& e) {
	in.setFromAttribute(e.a_useDBus,"useDBus");
	const SimpleNode* n = in.firstChild();
	bool hasChildren = n;
	while (n && in.getTagName() == "repository") {
		Repository v;
		in >> v;
		e.e_repository.push_back(v);
		n = in.nextSibling();
	}
	while (n && in.getTagName() == "filteringrules") {
		Filteringrules v;
		in >> v;
		e.e_filteringrules.push_back(v);
		n = in.nextSibling();
	}
	if (hasChildren) {
		in.parentNode();
	}
	return in;
}
StrigiDaemonConfiguration::StrigiDaemonConfiguration(const std::string& xml) {
	a_useDBus = false;
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const StrigiDaemonConfiguration& e) {
	out << "<strigiDaemonConfiguration";
	out << " useDBus='" << e.a_useDBus << "'";
	out << ">\n";
	std::list<Repository>::const_iterator repository_it;
	for (repository_it = e.e_repository.begin(); repository_it != e.e_repository.end(); repository_it++) {
		out << *repository_it;
	}
	std::list<Filteringrules>::const_iterator filteringrules_it;
	for (filteringrules_it = e.e_filteringrules.begin(); filteringrules_it != e.e_filteringrules.end(); filteringrules_it++) {
		out << *filteringrules_it;
	}
	out << "</strigiDaemonConfiguration>\n";

	return out;
}

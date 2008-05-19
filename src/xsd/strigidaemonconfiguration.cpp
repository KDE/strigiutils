/* DO NOT EDIT */
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
	in.setFromAttribute(e.a_name,"name");
	in.setFromAttribute(e.a_writeable,"writeable");
	in.setFromAttribute(e.a_pollingInterval,"pollingInterval");
	in.setFromAttribute(e.a_urlbase,"urlbase");
	in.setFromAttribute(e.a_indexdir,"indexdir");
	in.setFromAttribute(e.a_type,"type");
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
	a_writeable = false;
	a_pollingInterval = 0;
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Repository& e) {
	out << " <repository";
	out << " name='" << e.a_name << "'";
	out << " writeable='" << e.a_writeable << "'";
	out << " pollingInterval='" << e.a_pollingInterval << "'";
	out << " urlbase='" << e.a_urlbase << "'";
	out << " indexdir='" << e.a_indexdir << "'";
	out << " type='" << e.a_type << "'";
	out << ">\n";
	std::list<Path>::const_iterator path_it;
	for (path_it = e.e_path.begin(); path_it != e.e_path.end(); path_it++) {
		out << *path_it;
	}
	out << " </repository>\n";

	return out;
}
XMLStream&
operator>>(XMLStream& in, Filter& e) {
	in.setFromAttribute(e.a_pattern,"pattern");
	in.setFromAttribute(e.a_include,"include");
	return in;
}
Filter::Filter(const std::string& xml) {
	a_include = false;
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Filter& e) {
	out << " <filter";
	out << " pattern='" << e.a_pattern << "'";
	out << " include='" << e.a_include << "'";
	out << ">\n";
	out << " </filter>\n";

	return out;
}
XMLStream& operator>>(XMLStream&, Filter&);
XMLStream&
operator>>(XMLStream& in, Filters& e) {
	const SimpleNode* n = in.firstChild();
	bool hasChildren = n;
	while (n && in.getTagName() == "filter") {
		Filter v;
		in >> v;
		e.e_filter.push_back(v);
		n = in.nextSibling();
	}
	if (hasChildren) {
		in.parentNode();
	}
	return in;
}
Filters::Filters(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Filters& e) {
	out << " <filters";
	out << ">\n";
	std::list<Filter>::const_iterator filter_it;
	for (filter_it = e.e_filter.begin(); filter_it != e.e_filter.end(); filter_it++) {
		out << *filter_it;
	}
	out << " </filters>\n";

	return out;
}
XMLStream& operator>>(XMLStream&, Repository&);
XMLStream& operator>>(XMLStream&, Filters&);
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
	if (n && in.getTagName() == "filters") {
		in >> e.e_filters;
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
	out << e.e_filters;
	out << "</strigiDaemonConfiguration>\n";

	return out;
}

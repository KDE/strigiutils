/* DO NOT EDIT */
/* This file is generated from metadataproperties.xsd */
#include "metadataproperties.h"
#include <iostream>
XMLStream&
operator>>(XMLStream& in, Metadata& e) {
	in.setFromAttribute(e.a_key,"key");
	return in;
}
Metadata::Metadata(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Metadata& e) {
	out << " <metadata";
	out << " key='" << e.a_key << "'";
	out << ">\n";
	out << " </metadata>\n";

	return out;
}
XMLStream& operator>>(XMLStream&, Metadata&);
XMLStream&
operator>>(XMLStream& in, Metadataproperties& e) {
	const SimpleNode* n = in.firstChild();
	bool hasChildren = n;
	while (n && in.getTagName() == "metadata") {
		Metadata v;
		in >> v;
		e.e_metadata.push_back(v);
		n = in.nextSibling();
	}
	if (hasChildren) {
		in.parentNode();
	}
	return in;
}
Metadataproperties::Metadataproperties(const std::string& xml) {
	if (xml.length()) {
		XMLStream stream(xml);
		stream >> *this;
	}
}
std::ostream&
operator<<(std::ostream& out, const Metadataproperties& e) {
	out << "<metadataproperties";
	out << ">\n";
	std::list<Metadata>::const_iterator metadata_it;
	for (metadata_it = e.e_metadata.begin(); metadata_it != e.e_metadata.end(); metadata_it++) {
		out << *metadata_it;
	}
	out << "</metadataproperties>\n";

	return out;
}

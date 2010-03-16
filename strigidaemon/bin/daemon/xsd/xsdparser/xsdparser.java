package xsdparser;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.xml.XMLConstants;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.bootstrap.DOMImplementationRegistry;
import org.w3c.dom.ls.DOMImplementationLS;
import org.w3c.dom.ls.LSInput;
import org.w3c.dom.ls.LSParser;

public class xsdparser {
	public static void main(String[] args) throws Exception {
		if (args.length != 1) {
			return;
		}
		new xsdparser(args[0]);
	}

	public xsdparser(String xsdfile) throws Exception {
		System.setProperty(DOMImplementationRegistry.PROPERTY,
				"org.apache.xerces.dom.DOMImplementationSourceImpl");
		DOMImplementationRegistry registry = null;
		try {
			registry = DOMImplementationRegistry.newInstance();
		} catch (Exception e) {
		}
		if (registry == null) {
			System
					.setProperty(DOMImplementationRegistry.PROPERTY,
							"com.sun.org.apache.xerces.internal.dom.DOMImplementationSourceImpl");
		}
		try {
			registry = DOMImplementationRegistry.newInstance();
		} catch (Exception e) {
		}
		if (registry == null) {
			System.err.println("No DOMImplementationRegistry found.");
			System.err.println("Make sure your java runtime library contains one.");
			System.err.println("You may have multiple conflicting java implementations installed.");
			return;
		}
		DOMImplementationLS domimpl = (DOMImplementationLS) registry
				.getDOMImplementation("LS 3.0");
		LSParser parser = domimpl.createLSParser(
				DOMImplementationLS.MODE_SYNCHRONOUS, null);
		LSInput input = domimpl.createLSInput();
		input.setByteStream(new FileInputStream(xsdfile));

		Document document;
		try {
			document = parser.parse(input);
		} catch (Exception e) {
			e.printStackTrace();
			return;
		}

		StringBuilder testmaincodemain = new StringBuilder();

		Node n = document.getFirstChild();

		// check that this is 'schema'
		if (n == null
				|| !"schema".equals(n.getNodeName())
				|| !XMLConstants.W3C_XML_SCHEMA_NS_URI.equals(n
						.getNamespaceURI())) {
			System.out.println("problem " + n.getNamespaceURI() + " "
					+ XMLConstants.W3C_XML_SCHEMA_NS_URI);
		}
		n = n.getFirstChild();
		List<Class> classes = new ArrayList<Class>();
		while (n != null) {
			if (n instanceof Element) {
				Class c = parseElement((Element) n);
				if (c != null) {
					classes.addAll(c.getClasses());
					classes.add(c);
				}
			}
			n = n.getNextSibling();
		}

		// determine the base of the filename
		int pos = xsdfile.lastIndexOf(File.separator);
		String base;
		if (pos == -1) {
			base = xsdfile;
		} else {
			base = xsdfile.substring(pos + 1);
		}
		base = base.substring(0, base.indexOf('.'));

		// write the parsed classes
		PrintWriter header = new PrintWriter(new FileWriter(base + ".h"));
		header.println("/* DO NOT EDIT */");
		header.println("/* This file is generated from " + xsdfile + " */");
		header.println("#ifndef " + base.toUpperCase() + "_H");
		header.println("#define " + base.toUpperCase() + "_H");
                header.println("#include \"xsd/xsdparser/xmlstream.h\"");
		header.println("#include <string>");
		header.println("#include <list>");
		PrintWriter impl = new PrintWriter(new FileWriter(base + ".cpp"));
		impl.println("/* DO NOT EDIT */");
		impl.println("/* This file is generated from " + xsdfile + " */");
		impl.println("#include \"" + base + ".h\"");
		impl.println("#include <iostream>");
		for (Class c : classes) {
			writeHeader(c, header);
			writeImplementation(c, impl);

			// add code to the test executable
			String lcname = c.classname.toLowerCase();
			testmaincodemain.append("\tfilename << n++ << \".xml\";\n");
			testmaincodemain.append("\txml = read(filename.str());\n");
			testmaincodemain.append("\t" + c.classname + " " + lcname
					+ "(xml);\n");

			testmaincodemain
					.append("\tf.open(filename.str().c_str(), std::ios::binary);\n");
			testmaincodemain.append("\tf << " + lcname + ";\n");
			testmaincodemain.append("\tf.close();\n");
			testmaincodemain.append("\tfilename.str(\"\");\n\n");

		}
		header.println("#endif");
		header.close();
		impl.close();

		// write a test executable
		PrintWriter h = new PrintWriter(new FileWriter(base + "test.cpp"));
		h.println("/* This file is generated from " + xsdfile + " */");
		h.println("#include <iostream>");
		h.println("#include <fstream>");
		h.println("#include <sstream>");
		h.println("#include \"" + base + ".h\"");
		h.println("std::string\nread(const std::string& file) {");
		h.println("\tstd::stringbuf buf;");
		h.println("\tstd::ifstream f(file.c_str(), std::ios::binary);");
		h.println("\tf.get(buf, '\\0');");
		h.println("\tf.close();");
		h.println("\treturn buf.str();\n}");
		h.println("int\nmain() {");
		h.println("\tstd::string xml;");
		h.println("\tint n = 1;");
		h.println("\tstd::ofstream f;\n");
		h.println("\tstd::ostringstream filename;\n");
		h.println(testmaincodemain.toString());
		h.println("\treturn 0;\n}");
		h.close();
	}

	public Class parseElement(Element e) throws IOException {
		String name = e.getAttribute("name");
		if (name == null || name.length() == 0) {
			return null;
		}
		Class c = new Class();
		c.elementname = name;

		c.classname = name.toUpperCase().substring(0, 1) + name.substring(1);

		Node n = e.getFirstChild();
		while (n != null) {
			if (n instanceof Element) {
				if ("complexType".equals(n.getNodeName())) {
					Node nn = n.getFirstChild();
					while (nn != null) {
						if (nn instanceof Element) {
							Element ne = (Element) nn;
							if ("attribute".equals(ne.getNodeName())) {
								String atype = ne.getAttribute("type");
								atype = mapType(atype);
								String aname = ne.getAttribute("name");
								if (atype != null && aname != null) {
									c.atts.put(aname, atype);
								}
								// System.out.println(atype);
							} else if ("sequence".equals(ne.getNodeName())) {
								parseSequence(ne, c);
							}
						}
						nn = nn.getNextSibling();
					}
					break;
				}
			}
			n = n.getNextSibling();
		}

		return c;
	}

	void parseSequence(Element seq, Class c) throws IOException {
		Node n = seq.getFirstChild();
		while (n != null) {
			if (n instanceof Element) {
				Element ne = (Element) n;
				if ("element".equals(ne.getNodeName())) {
					String name = ne.getAttribute("name");
					String type = ne.getAttribute("type");
					int min = 1;
					int max = 1;
					try {
						min = Integer.parseInt(ne.getAttribute("minOccurs"));
					} catch (Exception e) {
					}
					try {
						max = Integer.parseInt(ne.getAttribute("maxOccurs"));
					} catch (Exception e) {
					}
					System.out.println("min " + min + " max " + max);
					type = mapType(type);
					if (name != null && name.length() > 0) {
						SequencePart part = new SequencePart();
						part.name = name.toLowerCase();
						part.tagname = name;
						part.seqtype = (min == 1 && max == 1) ? SequencePart.Type.SINGLE
								: SequencePart.Type.LIST;
						if (type != null) {
							part.type = type;
							System.out.println(name + " " + type);
							c.sequence.add(part);
						} else {
							Class nc = parseElement(ne);
							nc.depth = c.depth + 1;
							if (nc != null) {
								part.type = nc.classname;
								part.element = nc;
								System.out.println(name + " - " + part.type);
								c.sequence.add(part);
							}
						}
					}
				}
			}
			n = n.getNextSibling();
		}
	}

	String mapType(String type) {
		if ("int".equals(type)) {
			return "int";
		} else if ("string".equals(type)) {
			return "std::string";
		} else if ("boolean".equals(type)) {
			return "bool";
		}
		return null;
	}

	void writeHeader(Class c, PrintWriter h) throws IOException {
		h.println("class " + c.classname + " {");
		h.println("public:");
		h.println("\t" + c.classname + "(const std::string& xml = \"\");");
                h.println("\tfriend std::ostream& operator<<(std::ostream&, const "
				+ c.classname + "&);");
                if (c.classname.equals("StrigiDaemonConfiguration"))
                {
                    h.println("\tfriend XMLStream& operator>>(XMLStream& in, "
				+ c.classname + "&);");
                    h.println("protected:");
                }
		for (Entry<String, String> e : c.atts.entrySet()) {
			h.println("\t" + e.getValue() + " a_" + e.getKey() + ";");
		}
		for (SequencePart part : c.sequence) {
			if (part.seqtype == SequencePart.Type.SINGLE) {
				h.println("\t" + part.type + " e_" + part.name + ";");
			} else {
				h
						.println("\tstd::list<" + part.type + "> e_"
								+ part.name + ";");
			}
		}
                
		h.println("};");		
	}

	void writeImplementation(Class c, PrintWriter w) throws IOException {
		for (SequencePart part : c.sequence) {
			if (part.element != null) {
				w.println("XMLStream& operator>>(XMLStream&, "
						+ part.element.classname + "&);");
			}
		}
		w.println("XMLStream&\noperator>>(XMLStream& in, " + c.classname
				+ "& e) {");
		for (String name : c.atts.keySet()) {
			w.println("\tin.setFromAttribute(e.a_" + name + ",\"" + name
					+ "\");");
		}
		if (c.sequence.size() > 0) {
			w.println("\tconst SimpleNode* n = in.firstChild();");
			w.println("\tbool hasChildren = n;");
		}
		for (SequencePart part : c.sequence) {
			if (part.seqtype == SequencePart.Type.SINGLE) {
				w.println("\tif (n && in.getTagName() == \"" + part.tagname
						+ "\") {");
				w.println("\t\tin >> e.e_" + part.name + ";");
			} else {
				w.println("\twhile (n && in.getTagName() == \"" + part.tagname
						+ "\") {");
				w.println("\t\t" + part.type + " v;");
				w.println("\t\tin >> v;");
				w.println("\t\te.e_" + part.name + ".push_back(v);");
			}
			w.println("\t\tn = in.nextSibling();");
			w.println("\t}");
		}
		if (c.sequence.size() > 0) {
			w.println("\tif (hasChildren) {");
			w.println("\t\tin.parentNode();");
			w.println("\t}");
		}
		w.println("\treturn in;\n}");
		w.println(c.classname + "::" + c.classname
				+ "(const std::string& xml) {");
		for (Entry<String, String> e : c.atts.entrySet()) {
			if (e.getValue().equals("int")) {
				w.println("\ta_" + e.getKey() + " = 0;");
			} else if (e.getValue().equals("bool")) {
				w.println("\ta_" + e.getKey() + " = false;");
			}
		}
		w.println("\tif (xml.length()) {");
		w.println("\t\tXMLStream stream(xml);");
		w.println("\t\tstream >> *this;");
		w.println("\t}");
		w.println("}");
		w.println("std::ostream&\noperator<<(std::ostream& out, const "
				+ c.classname + "& e) {");
		char tabarray[] = new char[c.depth];
		java.util.Arrays.fill(tabarray, ' ');
		String tabs = new String(tabarray);
		w.println("\tout << \""+tabs+"<" + c.elementname + "\";");
		for (String name : c.atts.keySet()) {
			w.println("\tout << \" " + name + "='\" << e.a_" + name
					+ " << \"'\";");
		}
		w.println("\tout << \">\\n\";");
		for (SequencePart part : c.sequence) {
			if (part.seqtype == SequencePart.Type.SINGLE) {
				w.println("\tout << e.e_" + part.name + ";");
			} else {
				w.println("\tstd::list<" + part.type + ">::const_iterator "
						+ part.name + "_it;");
				w.println("\tfor (" + part.name + "_it = e.e_" + part.name
						+ ".begin(); " + part.name + "_it != e.e_" + part.name
						+ ".end(); " + part.name + "_it++) {");
				w.println("\t\tout << *" + part.name + "_it;\n\t}");
			}
		}
		w.println("\tout << \"" + tabs + "</" + c.elementname + ">\\n\";\n");
		w.println("\treturn out;\n}");
	}
}

class Class {
	int depth = 0;

	String elementname;

	String classname;

	List<SequencePart> sequence = new ArrayList<SequencePart>();

	Map<String, String> atts = new HashMap<String, String>();

	List<Class> getClasses() {
		List<Class> classes = new ArrayList<Class>();
		for (SequencePart part : sequence) {
			if (part.element != null) {
				System.out.println("sub " + part.element.classname);
				classes.addAll(part.element.getClasses());
				classes.add(part.element);
			}
		}
		return classes;
	}
}

class SequencePart {
	String name;
	String tagname;

	enum Type {
		SINGLE, LIST, SET
	}

	Type seqtype;

	String type;

	Class element;
}

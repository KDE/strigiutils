/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Alexandr Goncearenco <neksa@neksa.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <strigi/fieldtypes.h>
#include <strigi/fieldpropertiesdb.h>
#include <iostream>
#include <stdio.h>
using namespace Strigi;
using namespace std;

RegisteredField::RegisteredField(const string& k, const string& t, int m,
        const RegisteredField* p)
        : m_key(k), m_type(t), m_maxoccurs(m), m_parent(p), m_writerdata(0),
	  m_properties(FieldPropertiesDb::db().properties(k)) {
}

RegisteredField::RegisteredField(const string& fieldname):
    m_key(fieldname),
    m_type(FieldPropertiesDb::db().properties(fieldname).typeUri()), // obsolete - is never used
    m_maxoccurs(FieldPropertiesDb::db().properties(fieldname).maxCardinality()), // obsolete - is never used
    m_parent(0), // obsolete - is never used
    m_writerdata(0),
    m_properties(FieldPropertiesDb::db().properties(fieldname)) {
}

const string FieldRegister::floatType = "float";
const string FieldRegister::integerType = "integer";
const string FieldRegister::binaryType = "binary";
const string FieldRegister::stringType = "string";
const string FieldRegister::datetimeType = "datetime";

const string FieldRegister::pathFieldName = "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#url";
const string FieldRegister::parentLocationFieldName = "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#isPartOf";
const string FieldRegister::encodingFieldName = "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#characterSet";
const string FieldRegister::mimetypeFieldName = "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#mimeType";
const string FieldRegister::filenameFieldName = "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#fileName";
const string FieldRegister::contentFieldName = "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#plainTextContent";
const string FieldRegister::extensionFieldName = "http://freedesktop.org/standards/xesam/1.0/core#fileExtension";
const string FieldRegister::embeddepthFieldName = "http://strigi.sf.net/ontologies/0.9#depth";
const string FieldRegister::mtimeFieldName = "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#lastModified";
const string FieldRegister::sizeFieldName = "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#contentSize";
const string FieldRegister::typeFieldName = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";

const string FieldRegister::defaultNamespace = "http://strigi.sf.net/ontologies/0.9#";

FieldRegister::FieldRegister() {
    pathField = registerField(pathFieldName);
    parentLocationField = registerField(parentLocationFieldName);
    encodingField = registerField(encodingFieldName);//, stringType, 1, 0);
    mimetypeField = registerField(mimetypeFieldName);//, stringType, -1, 0);
    filenameField = registerField(filenameFieldName);//, stringType, 1, 0);
    extensionField = registerField(extensionFieldName);//, stringType, 1, 0);
    embeddepthField = registerField(embeddepthFieldName);//, integerType, 1, 0);
    mtimeField = registerField(mtimeFieldName);//, integerType, 1, 0);
    sizeField = registerField(sizeFieldName);//, integerType, 1, 0);
    typeField = registerField(typeFieldName);
    parseErrorField = registerField("http://strigi.sf.net/ontologies/0.9#debugParseError");
}

FieldRegister::~FieldRegister() {
    map<string, RegisteredField*>::const_iterator i;
    for (i = m_fields.begin(); i != m_fields.end(); ++i) {
        delete i->second;
    }
}
const RegisteredField*
FieldRegister::registerField(const string& fieldname,
        const string& type, int maxoccurs, const RegisteredField* parent) {
	
    //TODO (neksa) uncomment the warning.
    // fprintf(stderr, "WARNING: this FieldRegister::registerField call is obsolete. Use registerField(\"%s\") instead.\n",
    //		fieldname.c_str());
		
    // passed parameters type, maxoccurs and parent are just ignored
    // in exchange, properties loaded from fieldproperties database are used
    return registerField(fieldname);
}
const RegisteredField*
FieldRegister::registerField(const string& fieldname) {
    map<string, RegisteredField*>::iterator i = m_fields.find(fieldname);
    if (i == m_fields.end()) {
	// if an instance of the RegisteredField has never been created before:
        //  - check with the fieldpropertiesdb first
	//  	- if not in the DB, then db().(addField)
	//  - create an instance of RegisteredField,
	//    remember it for the future calls, and return the pointer
        const FieldProperties& props
            = FieldPropertiesDb::db().properties(fieldname);
        if (!props.valid()) {
            // cerr << "WARNING: field '" << fieldname << "' is not defined in "
            //    "any rdfs ontology database." << endl;
	    // creates a field with defaults (stringType and no parents)
            FieldPropertiesDb::db().addField(fieldname);
        }
        RegisteredField* f = new RegisteredField(fieldname);
        m_fields[fieldname] = f;
        return f;
    } else {
	// if an instance of the RegisteredField has already been associated
	// with the fieldname then return pointer to it
	return i->second;
    }    
}

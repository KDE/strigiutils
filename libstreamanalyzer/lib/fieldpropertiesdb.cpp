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
#include <strigi/fieldpropertiesdb.h>
#include "fieldproperties_private.h"
#include <strigi/fieldtypes.h>
#include <vector>
#include <map>
#include <iostream>
#include <iterator>
#include <set>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#else
#include "stgdirent.h"
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <libxml/SAX2.h>
#include <sys/stat.h>
#include <config.h>

#ifdef _MSC_VER
# define strcasecmp stricmp
#endif

using namespace Strigi;
using namespace std;

class FieldPropertiesDb::Private {
public:
    map<string, FieldProperties> properties;
    map<string, FieldProperties> propertiesByAlias;
    map<string, ClassProperties> classes;
    static const FieldProperties& emptyField();
    static const ClassProperties& emptyClass();

    Private();
    static vector<string> getdirs(const string&);
    static vector<string> getXdgDirs();
    void addEssentialProperties();
    void loadProperties(const string& dir);
    void parseProperties(FILE* f);
    void storeProperties(FieldProperties::Private& props);
    void warnIfLocale(const char* name, const string& locale);

// SAX Callbacks and stuff
    map<string, FieldProperties::Private> pProperties;
    map<string, ClassProperties::Private> pClasses;

    bool saxError;
    enum {defNone, defClass, defProperty} currentDefinition;
    string currentSubElement;
    string currentElementChars;
    string currentElementLang;
    string currentElementResource;
    bool nestedResource;
    FieldProperties::Private currentField;
    ClassProperties::Private currentClass;
    map<string, xmlEntity> xmlEntities;

    void setDefinitionAttribute(const char* name, size_t namelen,
        const char * value, size_t valuelen);

    static void charactersSAXFunc(void* ctx, const xmlChar * ch, int len);
    static void errorSAXFunc(void* ctx, const char * msg, ...);
    static void startElementNsSAX2Func(void * ctx,
        const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
        int nb_namespaces, const xmlChar ** namespaces, int nb_attributes,
        int nb_defaulted, const xmlChar ** attributes);
    static void endElementNsSAX2Func(void *ctx,
        const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);
    static xmlEntityPtr getEntitySAXFunc(void * ctx, const xmlChar * name);
    static void xmlSAX2EntityDecl(void * ctx, const xmlChar * name, int type,
        const xmlChar * publicId, const xmlChar * systemId, xmlChar * content);

    static bool isBoolValid(const char *uri, const char* name,
        const char* value, bool& result);
};

const FieldProperties&
FieldPropertiesDb::Private::emptyField() {
    static const FieldProperties e;
    return e;
}
const ClassProperties&
FieldPropertiesDb::Private::emptyClass() {
    static const ClassProperties f;
    return f;
}

FieldPropertiesDb&
FieldPropertiesDb::db() {
    static FieldPropertiesDb db;
    return db;
}
FieldPropertiesDb::FieldPropertiesDb() :p(new Private()) {
}
FieldPropertiesDb::~FieldPropertiesDb() {
    delete p;
}
const FieldProperties&
FieldPropertiesDb::properties(const std::string& uri) const {
    map<std::string, FieldProperties>::const_iterator j
        = p->properties.find(uri);
    if (j == p->properties.end()) {
        return FieldPropertiesDb::Private::emptyField();
    } else {
        return j->second;
    }
}

const FieldProperties&
FieldPropertiesDb::propertiesByAlias(const std::string& alias) const {
    map<std::string, FieldProperties>::const_iterator j
        = p->propertiesByAlias.find(alias);
    if (j == p->propertiesByAlias.end()) {
        return FieldPropertiesDb::Private::emptyField();
    } else {
        return j->second;
    }
}

const map<string, FieldProperties>&
FieldPropertiesDb::allProperties() const {
    return p->properties;
}

const ClassProperties&
FieldPropertiesDb::classes(const std::string& uri) const {
    map<std::string, ClassProperties>::const_iterator j = p->classes.find(uri);
    if (j == p->classes.end()) {
        return FieldPropertiesDb::Private::emptyClass();
    } else {
        return j->second;
    }
}
const map<string, ClassProperties>&
FieldPropertiesDb::allClasses() const {
    return p->classes;
}
vector<string>
FieldPropertiesDb::Private::getdirs(const string& direnv) {
    vector<string> dirs;
    string::size_type lastp = 0;
    string::size_type p = direnv.find(':');
    while (p != string::npos) {
        dirs.push_back(direnv.substr(lastp, p-lastp));
        lastp = p+1;
        p = direnv.find(':', lastp);
     }
    dirs.push_back(direnv.substr(lastp));
    return dirs;
}
vector<string>
FieldPropertiesDb::Private::getXdgDirs() {
    // find the XDG HOME directories or if not defined the local HOME dirs
    vector<string> dirs;
    const char* dirpath = getenv("XDG_DATA_HOME");
    if (dirpath) {
        dirs = getdirs(dirpath);
    } else {
        dirpath = getenv("HOME");
        if (dirpath) {
             dirs = getdirs(string(dirpath)+"/.local/share");
        }
    }
    // add the XDG DATA directories or if not defined the default dirs
    dirpath = getenv("XDG_DATA_DIRS");
    vector<string> d;
    if (dirpath) {
        d = getdirs(dirpath);
    } else {
        d = getdirs(INSTALLDIR "/share:/usr/local/share:/usr/share");
    }
    copy(d.begin(), d.end(), back_insert_iterator<vector<string> >(dirs));
    return dirs;
}
FieldPropertiesDb::Private::Private() {
    // some properties are defined hard in the code because they are essential
    addEssentialProperties();

    vector<string> dirs = getXdgDirs();
    vector<string>::const_iterator i;
    set<string> done;
    for (i=dirs.begin(); i!=dirs.end(); ++i) {
        if (done.find(*i) == done.end()) {
            done.insert(*i);
            loadProperties(*i);
        }
    }

    // Generate childUris, applicable* and locales values.

    for (map<string, FieldProperties::Private>::const_iterator prop
            = pProperties.begin();
            prop != pProperties.end(); ++prop) {
        FieldProperties::Private property = prop->second;

        for (map<string,FieldProperties::Localized>::iterator l
                = property.localized.begin();
                l != property.localized.end(); ++l) {
            property.locales.push_back(l->first);
        }

        const vector<string>& parents = property.parentUris;
        for (vector<string>::const_iterator parent = parents.begin();
                parent != parents.end(); ++parent ) {
            pProperties[*parent].childUris.push_back(property.uri);
        }
        const vector<string>& applicable = property.applicableClasses;
        for (vector<string>::const_iterator aclass = applicable.begin();
                aclass != applicable.end(); ++aclass ) {
            pClasses[*aclass].applicableProperties.push_back(property.uri);
        }
    }

    for (map<string, ClassProperties::Private>::const_iterator aclass
            = pClasses.begin();
            aclass != pClasses.end(); ++aclass) {
        ClassProperties::Private cclass = aclass->second;

        for (map<string,ClassProperties::Localized>::iterator l
                = cclass.localized.begin();
                l != cclass.localized.end(); ++l) {
            cclass.locales.push_back(l->first);
        }

        const vector<string>& parents = cclass.parentUris;
        for (vector<string>::const_iterator parent = parents.begin();
            parent != parents.end(); ++parent ) {
            pClasses[*parent].childUris.push_back(cclass.uri);
        }
    }

    copy(pClasses.begin(), pClasses.end(), inserter(classes, classes.end()) );

    // Construct properties and propertiesByAlias lists
    for (map<string, FieldProperties::Private>::const_iterator prop
            = pProperties.begin(); prop != pProperties.end(); ++prop) {
        FieldProperties property(prop->second);
        string alias = prop->second.alias;

        if(alias.size()) {
            if(propertiesByAlias.find(alias) == propertiesByAlias.end()) {
                propertiesByAlias[alias] = property;
            } else {
//FIXME: use loging framework
//                cerr << "Error: alias " << alias << " requested by several properties: " << propertiesByAlias.find(alias)->second.uri()
//		  << ", " << prop->second.uri
//		  << endl;
            }
        }

        if (properties.find(property.uri()) == properties.end()) {
            properties[property.uri()] = property;
        }
    }

    pProperties.clear();
    pClasses.clear();
}

// FIXME (phreedom): should not directly fill properties[]
//   not all properties from fieldtypes.* are created
void
FieldPropertiesDb::Private::addEssentialProperties() {
    FieldProperties::Private props;  
    props.stored = true;

    props.typeuri = FieldRegister::datetimeType;
    props.uri = FieldRegister::mtimeFieldName;
    properties[FieldRegister::mtimeFieldName] = props;

    props.typeuri = FieldRegister::integerType;
    props.uri = FieldRegister::sizeFieldName;
    properties[FieldRegister::sizeFieldName] = props;

    props.uri = FieldRegister::embeddepthFieldName;
    properties[FieldRegister::embeddepthFieldName] = props;

    props.typeuri = FieldRegister::stringType;
    props.uri = FieldRegister::pathFieldName;
    props.tokenized = false; // should not be tokenized: needed for retrieval 
    properties[FieldRegister::pathFieldName] = props;

    props.uri = FieldRegister::filenameFieldName;
    props.tokenized = true;
    properties[FieldRegister::filenameFieldName] = props;

    props.uri = FieldRegister::mimetypeFieldName;
    properties[FieldRegister::mimetypeFieldName] = props;

    props.uri = FieldRegister::parentLocationFieldName;
    props.tokenized = false; // should not be tokenized: needed for retrieval
    properties[FieldRegister::parentLocationFieldName] = props;
}
void
FieldPropertiesDb::Private::loadProperties(const string& dir) {
    string pdir = dir + "/strigi/fieldproperties/";
    DIR* d = opendir(pdir.c_str());
    if (!d) {
        pdir = dir;
        d = opendir(pdir.c_str());
    }
    if (!d) {
        return;
    }
    if (pdir[pdir.length()-1] != '/') {
        pdir.append("/");
    }
    struct dirent* de = readdir(d);
    struct stat s;
    while (de) {
        string path(pdir+de->d_name);
        if (path.length() >= 5 && path.compare(path.length() - 5, 5, ".rdfs", 5) == 0 &&
                !stat(path.c_str(), &s) && S_ISREG(s.st_mode)) {
            FILE* f = fopen(path.c_str(), "r");
            if (f) {
                parseProperties(f);
                fclose(f);
            }
        }
        de = readdir(d);
    }
    closedir(d);
}
namespace {
int
strigi_xmlFileRead(void* context, char* buffer, int len) {
    FILE* f = (FILE*)context;
    return (int)fread(buffer, 1, len, f);
}
int
strigi_xmlFileClose(void*) {
    return 0;
}
}
void
FieldPropertiesDb::Private::parseProperties(FILE* f) {
    FieldProperties::Private props;

    xmlParserCtxtPtr ctxt;
    xmlSAXHandler handler;

    memset(&handler, 0, sizeof(xmlSAXHandler));
    handler.initialized = XML_SAX2_MAGIC;
    handler.characters = charactersSAXFunc;
    handler.error = errorSAXFunc;
    handler.startElementNs = startElementNsSAX2Func;
    handler.endElementNs = endElementNsSAX2Func;
    handler.getEntity = getEntitySAXFunc;
    handler.entityDecl = xmlSAX2EntityDecl;

    saxError = false;
    currentSubElement = "";
    currentElementChars = "";
    currentField.clear();
    currentClass.clear();
    currentDefinition = defNone;
    nestedResource = false;
    
    ctxt = xmlCreateIOParserCtxt(&handler, this, strigi_xmlFileRead, strigi_xmlFileClose, f, XML_CHAR_ENCODING_NONE);
    if (ctxt == 0) {
        saxError = true;
    } else {
        xmlCtxtUseOptions(ctxt, XML_PARSE_NOENT);
        if (xmlParseDocument(ctxt)) {
            saxError = true;
        }
    }

    if(saxError) {
//FIXME: use logging framework
//        cerr << "saxError in FieldPropertiesDB::parseProperties." << endl;
    }

    xmlFreeDoc(ctxt->myDoc);
    xmlFreeParserCtxt(ctxt);

    for (map<std::string, xmlEntity>::iterator j=xmlEntities.begin();
            j!=xmlEntities.end(); ++j) {
        delete [] j->second.name;
        delete [] j->second.content;
    }
    xmlEntities.clear();
}
void
FieldPropertiesDb::Private::xmlSAX2EntityDecl(void * ctx, const xmlChar * name,
        int type, const xmlChar* publicId, const xmlChar* systemId,
        xmlChar* content) {
    Private* p = (Private*)ctx;
    string stdname((const char*)name);
    map<std::string, xmlEntity>::const_iterator j
        = p->xmlEntities.find(stdname);
    if (j == p->xmlEntities.end()) {
        xmlEntity& newEntity = p->xmlEntities[stdname];
        newEntity.type = XML_ENTITY_DECL;
        newEntity.name = (xmlChar*)new char[stdname.size()+1];
        strcpy((char*)newEntity.name, stdname.c_str());
        newEntity.length = (int)strlen((const char*)content);
        newEntity.orig = (xmlChar*)new char[newEntity.length+1];
        strcpy((char*)newEntity.orig, (const char*)content);
        newEntity.content = newEntity.orig;
        newEntity.etype = XML_INTERNAL_GENERAL_ENTITY;
        newEntity.URI = newEntity.orig;
    } else {
// FIXME: use logging framework
//        cerr << "Error: entity " << name << " redeclared." << endl;
    }
}

xmlEntityPtr
FieldPropertiesDb::Private::getEntitySAXFunc(void * ctx, const xmlChar * name) {
    Private* p = (Private*)ctx;
    map<std::string, xmlEntity>::iterator j
        = p->xmlEntities.find((const char *)name);
    if (j == p->xmlEntities.end()) {
        return NULL;
    } else {
        return &j->second;
    }
}

void
FieldPropertiesDb::Private::charactersSAXFunc(void* ctx, const xmlChar* ch,
        int len) {
    Private* p = (Private*)ctx;
    p->currentElementChars.append((const char *)ch, len);
}

void
FieldPropertiesDb::Private::errorSAXFunc(void* ctx, const char* msg, ...) {
    Private* p = (Private*)ctx;
    p->saxError = true;
    string e;

    va_list args;
    va_start(args, msg);
    e += string(" ")+va_arg(args,char*);
    va_end(args);
// FIXME: use logging framework
//    cerr << "Error: " << e << endl;
}

bool
FieldPropertiesDb::Private::isBoolValid(const char *uri, const char* name,
        const char* value, bool& result){
    while (isspace(*value)) value++;
    if (strcasecmp(value,"false") == 0) {
        result = false;
    } else if (strcasecmp(value,"true") == 0) {
        result = true;
    } else {
// FIXME: use logging framework
//	 cerr << name << " flag value[" << value << "] for " << uri
//            << " is unrecognized. Should be in set {True,False}." << endl;
        return false;
    }
    return true;
}
namespace {
// due to inlining, the strlen(B) is evaluated at runtime when B is a literal
// string
inline bool compare(const char* a, size_t alen, const char* b) {
    return alen == strlen(b) && strncmp(a, b, strlen(b)) == 0;
}
}
void
FieldPropertiesDb::Private::setDefinitionAttribute(const char* name,
         size_t namelen, const char* value, size_t valuelen) {
    bool boolValue;
    //Trim leading and trailing whitespace
    size_t trimmedvallen = valuelen - strspn(value, " \t\n");
    string val(value + valuelen - trimmedvallen, trimmedvallen);
    val.erase(val.find_last_not_of(" \t\n") + 1);

    if (currentDefinition == defProperty) {
        if (compare(name, namelen, "about")) {
            warnIfLocale(val.c_str(), currentElementLang);
            if (currentField.uri.size()) {
// FIXME: use logging framework
//                cerr << "Uri is already defined for " << currentField.uri << "."
//                   << endl;
            } else {
                currentField.uri.assign(val);
            }
        } else if (compare(name, namelen, "alias")) {
            warnIfLocale(val.c_str(), currentElementLang);
            if (currentField.alias.size()) {
// FIXME: use logging framework
//                cerr << "alias is already defined for " << currentField.uri << "."
//                   << endl;
            } else {
                currentField.alias.assign(val);
            }
        } else if (compare(name, namelen, "range")) {
            warnIfLocale(currentField.uri.c_str(), currentElementLang);
            if (currentField.typeuri.size()) {
// FIXME: use logging framework
//               cerr << "range is already defined for " << currentField.uri
//                   << "." << endl;
            } else {
                currentField.typeuri.assign(currentElementResource);
            }
        } else if (compare(name, namelen, "label")) {
            if (currentElementLang.size()) {
                FieldProperties::Localized l(
                    currentField.localized[currentElementLang]);
                if (l.name.size()) {
// FIXME: use logging framework
//                    cerr << "label [" << currentElementLang
//                        << "] is already defined for " << currentField.uri
//                        << "." << endl;
                } else {
                    l.name.assign(val);
                    currentField.localized[currentElementLang] = l;
                }
            } else if (currentField.name.size()) {
// FIXME: use logging framework
//                cerr << "label is already defined for " << currentField.uri
//                    << "." << endl;
            } else {
                currentField.name.assign(val);
            }
        } else if (compare(name, namelen, "comment")) {
            if (currentElementLang.size()) {
                FieldProperties::Localized l(
                    currentField.localized[currentElementLang]);
                if (l.description.size()) {
// FIXME: use logging framework
//                    cerr << "comment[" << currentElementLang
//                        << "] is already defined for " << currentField.uri
//                        << "." << endl;
                } else {
                    l.description.assign(val);
                    currentField.localized[currentElementLang] = l;
                }
            } else if (currentField.description.size()) {
// FIXME: use logging framework
//                cerr << "comment is already defined for " << currentField.uri
//                    << "." << endl;
            } else {
                currentField.description.assign(val);
            }
        } else if (compare(name, namelen, "subPropertyOf")) {
            currentField.parentUris.push_back(currentElementResource);
        } else if (compare(name, namelen, "domain")) {
            currentField.applicableClasses.push_back(currentElementResource);
        } else if (compare(name, namelen, "binary")) {
            if (isBoolValid(currentField.uri.c_str(), "binary", value,
                    boolValue)) {
                currentField.binary = boolValue;
            }
        } else if (compare(name, namelen, "compressed")) {
            if (isBoolValid(currentField.uri.c_str(), "compressed", value,
                    boolValue)) {
                currentField.compressed = boolValue;
            }
        } else if (compare(name, namelen, "indexed")) {
            if (isBoolValid(currentField.uri.c_str(), "indexed", value,
                    boolValue)) {
                currentField.indexed = boolValue;
            }
        } else if (compare(name, namelen, "stored")) {
            if (isBoolValid(currentField.uri.c_str(), "stored", value,
                    boolValue)) {
                currentField.stored = boolValue;
            }
        } else if (compare(name, namelen, "tokenized")) {
            if (isBoolValid(currentField.uri.c_str(), "tokenized", value,
                    boolValue)) {
                currentField.tokenized = boolValue;
            }
        } else if (compare(name, namelen, "minCardinality")) {
            currentField.min_cardinality = atoi(value);
        } else if (compare(name, namelen, "maxCardinality")) {
            currentField.max_cardinality = atoi(value);
        }
    } else if (currentDefinition == defClass) {
        if (compare(name, namelen, "about")) {
            warnIfLocale(val.c_str(), currentElementLang);
            if (currentClass.uri.size()) {
// FIXME: use logging framework
//                cerr << "Uri is already defined for " << currentClass.uri
//                    << "." << endl;
            }
        } else {
            currentClass.uri.assign(val);
        }
    } else if (compare(name, namelen, "label")) {
        if (currentElementLang.size()) {
            ClassProperties::Localized l(
                currentClass.localized[currentElementLang]);
            if (l.name.size()) {
// FIXME: use logging framework
//                cerr << "label[" << currentElementLang
//                    << "] is already defined for " << currentClass.uri
//                    << "." << endl;
            } else {
                l.name.assign(val);
                currentClass.localized[currentElementLang] = l;
            }
        } else if (currentClass.name.size()) {
// FIXME: use logging framework
//            cerr << "label is already defined for " << currentClass.uri
//                << "." << endl;
        } else {
            currentClass.name.assign(val);
        }
    } else if (compare(name, namelen, "comment")) {
        if (currentElementLang.size()) {
            ClassProperties::Localized l(
                    currentClass.localized[currentElementLang]);
            if (l.description.size()) {
// FIXME: use logging framework
//                cerr << "comment[" << currentElementLang
//                    << "] is already defined for " << currentClass.uri
//                    << "." << endl;
            } else {
                l.description.assign(val);
                currentClass.localized[currentElementLang] = l;
            }
        } else if (currentField.description.size()) {
// FIXME: use logging framework
//            cerr << "comment is already defined for " << currentClass.uri
//                << "." << endl;
        } else {
            currentClass.description.assign(val);
        }
    } else if(compare(name, namelen, "subClassOf")) {
        currentClass.parentUris.push_back(currentElementResource);
    }
}

void
FieldPropertiesDb::Private::startElementNsSAX2Func(void * ctx,
        const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
        int nb_namespaces, const xmlChar ** namespaces, int nb_attributes,
        int nb_defaulted, const xmlChar ** attributes) {
    Private* p = (Private*)ctx;

    if (p->currentDefinition == defNone) {
        if (strcmp((const char *)localname, "Property") == 0) {
            p->currentDefinition = defProperty;
        } else if (strcmp((const char *)localname, "Class") == 0) {
            p->currentDefinition = defClass;
        }
        if (p->currentDefinition != defNone) {
            for (int i=0; i<nb_attributes; i++) {
                const xmlChar ** a = attributes + i*5;
                p->setDefinitionAttribute((const char*)a[0],
                    strlen((const char*)a[0]), (const char*)a[3], a[4]-a[3]);
            }
        }
    } else {
	if ( (strcmp((const char *)localname, "Property") == 0) ||
	      (strcmp((const char *)localname, "Class") == 0) ) {
	    p->nestedResource = true;
	} else {
	  p->currentSubElement = (const char *)localname;
	}
        for (int i=0; i<nb_attributes; i++) {
            const xmlChar ** a = attributes + i*5;
            size_t len = a[0]-a[1];
            if (len == 8 && strncmp((const char*)attributes[i*5], "resource", 8) == 0) {
                p->currentElementResource.assign((const char*)a[3], a[4]-a[3]);
            } else if (strcmp((const char*)attributes[i*5], "about") == 0) {
                p->currentElementResource.assign((const char*)a[3], a[4]-a[3]);
            } else if (strcmp((const char*)attributes[i*5], "lang") == 0) {
                p->currentElementLang.assign((const char*)a[3], a[4]-a[3]);
            }
        }
    }
}

void
FieldPropertiesDb::Private::endElementNsSAX2Func(void *ctx,
        const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {
    Private *p = (Private *) ctx;

    if (p->currentDefinition!=defNone) {
        if (strcmp((const char *)localname, "Property") == 0) {
	    if (p->nestedResource) {
		p->nestedResource = false;
	    } else {
		if (p->currentField.uri.size()) {
		    if(!p->currentField.alias.size()) {
			size_t pos;
			if ((pos = p->currentField.uri.rfind('#')) != string::npos){
			    p->currentField.alias = p->currentField.uri.substr(pos+1);
			}
		    }
		    p->pProperties[p->currentField.uri] = p->currentField;
		    p->currentField.clear();
		}
		p->currentDefinition = defNone;
	    }
        } else if (strcmp((const char *)localname, "Class") == 0) {
	    if (p->nestedResource) {
		p->nestedResource = false;
	    } else {
		if (p->currentClass.uri.size()) {
		    p->pClasses[p->currentClass.uri] = p->currentClass;
		    p->currentClass.clear();
		}
		p->currentDefinition = defNone;
	    }
        } else {
            if (p->currentSubElement == (const char*)localname) {
                p->setDefinitionAttribute(p->currentSubElement.c_str(),
                    p->currentSubElement.size(),
                    p->currentElementChars.c_str(),
                    p->currentElementChars.size());
                p->currentSubElement = "";
                p->currentElementChars = "";
                p->currentElementResource = "";
                p->currentElementLang = "";
            } else {
// FIXME: use logging framework
//                cerr << "ERROR: Wrong closing element " << localname
//                    << "." << endl;
            }
        }
    }
}

void
FieldPropertiesDb::Private::storeProperties(FieldProperties::Private& p) {
    if (p.uri.size()) {
        properties[p.uri] = p;
    }
    p.clear();
}

void
FieldPropertiesDb::Private::warnIfLocale(const char* name, const string& locale) {
    if (locale.size()) {
// FIXME: use logging framework
//        cerr << "Warning: you cannot define a locale for the resource URI "
//            << name << "." << endl;
    }
}
void
FieldPropertiesDb::addField(const std::string& key, const std::string& type,
        const std::string& parent) {
    FieldProperties::Private props;
    props.uri = key;
    props.typeuri = type;
    if (parent.size()) {
        props.parentUris.push_back(parent);
    }
    p->properties[key] = props;
}
void
FieldPropertiesDb::addField(const std::string& key) {
    FieldProperties::Private props;
    props.uri = key;
    props.typeuri = FieldRegister::stringType;
    p->properties[key] = props;
}
void
FieldProperties::Private::clear() {
    uri.clear();
    name.clear();
    alias.clear();
    description.clear();
    localized.clear();
    locales.clear();
    typeuri.clear();
    parentUris.clear();
    childUris.clear();
    applicableClasses.clear();

    indexed = true;
    stored = true;
    tokenized = true;
    compressed = false;
    binary = false;

    min_cardinality = 0;
    max_cardinality = -1; /** unlimited */
}
void
ClassProperties::Private::clear() {
    uri.clear();
    name.clear();
    description.clear();
    localized.clear();
    locales.clear();
    parentUris.clear();
    childUris.clear();
    applicableProperties.clear();
}

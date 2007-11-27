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
#include "fieldpropertiesdb.h"
#include "fieldproperties_private.h"
#include "fieldtypes.h"
#include <vector>
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
#include <cstdlib>
#include <cstring>
#include <map>
#include <iostream>
#include <list>
#include <set>

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
    void parseProperties(char*data);
    void storeProperties(FieldProperties::Private& props);
    void warnIfLocale(const char* name, string& locale);

// SAX Callbacks and stuff
    map<string, FieldProperties::Private> pProperties;
    map<string, ClassProperties::Private> pClasses;

    bool saxError;
    enum {defNone, defClass, defProperty} currentDefinition;
    string currentSubElement;
    string currentElementChars;
    string currentElementLang;
    string currentElementResource;
    FieldProperties::Private currentField;
    ClassProperties::Private currentClass;
    map<string,xmlEntity*> xmlEntities;
    list< pair<string, string> > entities;

    void setDefinitionAttribute(const char * name, const char * value);

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
    void replaceEntities(string& value);

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
    for (i=dirs.begin(); i!=dirs.end(); i++) {
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
    for (map<string, FieldProperties::Private>::const_iterator prop = pProperties.begin();
            prop != pProperties.end(); ++prop) {
        FieldProperties property(prop->second);
        string alias = prop->second.alias;

        if(alias.size()) {
            if(propertiesByAlias.find(alias) == propertiesByAlias.end()) {
                propertiesByAlias[alias] = property;
            } else {
                cerr << "Error: alias " << alias << " requested by several properties" << endl;
            }
        }

        properties[property.uri()] = property;
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
/*
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
    properties[FieldRegister::pathFieldName] = props;

    props.uri = FieldRegister::filenameFieldName;
    properties[FieldRegister::filenameFieldName] = props;

    props.uri = FieldRegister::mimetypeFieldName;
    properties[FieldRegister::mimetypeFieldName] = props;

    props.uri = FieldRegister::parentLocationFieldName;
    props.tokenized = false;
    properties[FieldRegister::parentLocationFieldName] = props;*/
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
    char* data = 0;
    while (de) {
        string path(pdir+de->d_name);
        if (path.length() >= 5 && path.substr(path.length()-5) == ".rdfs" &&
                !stat(path.c_str(), &s) && S_ISREG(s.st_mode)) {
            FILE* f = fopen(path.c_str(), "r");
            if (f) {
                // read the entire file at once
                data = (char*)realloc(data, s.st_size+1);
                if (fread(data, 1, s.st_size, f) == (size_t)s.st_size) {
                    //fprintf(stderr, "parsing %s\n", path.c_str());
                    data[s.st_size] = '\0';
                    parseProperties(data);
                }
                fclose(f);
            }
        }
        de = readdir(d);
    }
    closedir(d);
    free(data);
}
void
FieldPropertiesDb::Private::parseProperties(char* data) {
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

    ctxt = xmlCreatePushParserCtxt(&handler, this, data, strlen(data), "xxx");
    if (ctxt == 0 || xmlParseChunk(ctxt, 0, 0, 1)) {
        saxError = true;
    }

    if(saxError) {
        cerr << "saxError in FieldPropertiesDB::parseProperties." << endl;
    }

    xmlFreeParserCtxt(ctxt);
    entities.clear();

    for (map<std::string, xmlEntityPtr>::iterator j=xmlEntities.begin();
            j!=xmlEntities.end(); ++j) {
        delete [] j->second->name;
        delete [] j->second->content;
        delete j->second;
    }
    xmlEntities.clear();
}
void
FieldPropertiesDb::Private::xmlSAX2EntityDecl(void * ctx, const xmlChar * name,
        int type, const xmlChar* publicId, const xmlChar* systemId,
        xmlChar* content) {
    Private* p = (Private*)ctx;
    xmlEntityPtr newEntity;
    char * str;
    int len;
    map<std::string, xmlEntityPtr>::const_iterator j
        = p->xmlEntities.find((const char *)name);
    if (j == p->xmlEntities.end()) {
        newEntity = new xmlEntity;
        memset(newEntity, 0 , sizeof(xmlEntity));
        newEntity->type = XML_ENTITY_DECL;

        len=strlen((const char*)name);
        str = new char[len+1];
        strcpy(str, (const char*)name);
        newEntity->name = (const xmlChar *)str;
        len = strlen((const char *)content);
        newEntity->length = len;
        str = new char[len+1];
        strcpy(str, (const char *)content);
        newEntity->orig = (xmlChar *)str;
        newEntity->content = (xmlChar *)str;
        newEntity->etype = XML_INTERNAL_GENERAL_ENTITY;
        newEntity->URI = (xmlChar *)str;

        p->xmlEntities[(const char *)name] = newEntity;
        p->entities.push_back(
            pair<string,string>((const char *)name, (const char *)content));
    } else {
        cerr << "Error: entity " << name << " redeclared." << endl;
    }
}

xmlEntityPtr
FieldPropertiesDb::Private::getEntitySAXFunc(void * ctx, const xmlChar * name) {
    Private* p = (Private*)ctx;
    map<std::string, xmlEntityPtr>::const_iterator j
        = p->xmlEntities.find((const char *)name);
    if (j == p->xmlEntities.end()) {
        return NULL;
    } else {
        return j->second;
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
    cerr << "Error: " << e << endl;
}

bool
FieldPropertiesDb::Private::isBoolValid(const char *uri, const char* name,
        const char* value, bool& result){
    if (strcasecmp(value,"false") == 0) {
        result = false;
    } else if (strcasecmp(value,"true") == 0) {
        result = true;
    } else {
        cerr << name << " flag value[" << value << "] for " << uri
            << " is unrecognized. Should be in set {True,False}." << endl;
        return false;
    }
    return true;
}

// workaround for entities handling problem with libxml
void
FieldPropertiesDb::Private::replaceEntities(string& value) {
    for (list< pair<string, string> >::const_iterator j = entities.begin();
            j != entities.end(); ++j) {
        string entity = '&' + j->first + ';';
        size_t pos = string::npos;
        while((pos = value.find(entity)) != string::npos) {
        value.erase(pos, entity.size());
            value.insert(pos, j->second);
        }
    }
}

void
FieldPropertiesDb::Private::setDefinitionAttribute(const char * name,
         const char * value) {
    bool boolValue;
    //Trim leading and trailing whitespace
    string val = value;
    val.erase(0, val.find_first_not_of(" \t\n"));
    val.erase(val.find_last_not_of(" \t\n") + 1);

    replaceEntities(val);

    if (currentDefinition == defProperty) {
        if (strcmp(name, "about") == 0) {
            warnIfLocale(val.c_str(), currentElementLang);
            if (currentField.uri.size()) {
                cerr << "Uri is already defined for " << currentField.uri << "."
                   << endl;
            } else {
                currentField.uri.assign(val);
            }
        } else if (strcmp(name, "alias") == 0) {
            warnIfLocale(val.c_str(), currentElementLang);
            if (currentField.alias.size()) {
                cerr << "alias is already defined for " << currentField.uri << "."
                   << endl;
            } else {
                currentField.alias.assign(val);
            }
        } else if(strcmp(name, "range") == 0) {
            warnIfLocale(currentField.uri.c_str(), currentElementLang);
            if (currentField.typeuri.size()) {
               cerr << "range is already defined for " << currentField.uri
                   << "." << endl;
            } else {
                replaceEntities(currentElementResource);
                currentField.typeuri.assign(currentElementResource);
            }
        } else if(strcmp(name, "label") == 0) {
            if (currentElementLang.size()) {
                FieldProperties::Localized l(
                    currentField.localized[currentElementLang]);
                if (l.name.size()) {
                    cerr << "label [" << currentElementLang
                        << "] is already defined for " << currentField.uri
                        << "." << endl;
                } else {
                    l.name.assign(val);
                    currentField.localized[currentElementLang] = l;
                }
            } else if (currentField.name.size()) {
                cerr << "label is already defined for " << currentField.uri
                    << "." << endl;
            } else {
                currentField.name.assign(val);
            }
        } else if (strcmp(name, "comment") == 0) {
            if (currentElementLang.size()) {
                FieldProperties::Localized l(
                    currentField.localized[currentElementLang]);
                if (l.description.size()) {
                    cerr << "comment[" << currentElementLang
                        << "] is already defined for " << currentField.uri
                        << "." << endl;
                } else {
                    l.description.assign(val);
                    currentField.localized[currentElementLang] = l;
                }
            } else if (currentField.description.size()) {
                cerr << "comment is already defined for " << currentField.uri
                    << "." << endl;
            } else {
                currentField.description.assign(val);
            }
        } else if (strcmp(name, "subPropertyOf") == 0) {
            replaceEntities(currentElementResource);
            currentField.parentUris.push_back(currentElementResource);
        } else if (strcmp(name, "domain") == 0) {
            replaceEntities(currentElementResource);
            currentField.applicableClasses.push_back(currentElementResource);
        } else if (strcmp(name, "binary") == 0) {
            if (isBoolValid(currentField.uri.c_str(), "binary", value,
                    boolValue)) {
                currentField.binary = boolValue;
            }
        } else if (strcmp(name, "compressed") == 0) {
            if (isBoolValid(currentField.uri.c_str(), "compressed", value,
                    boolValue)) {
                currentField.compressed = boolValue;
            }
        } else if (strcmp(name, "indexed") == 0) {
            if (isBoolValid(currentField.uri.c_str(), "indexed", value,
                    boolValue)) {
                currentField.indexed = boolValue;
            }
        } else if (strcmp(name, "stored") == 0) {
            if (isBoolValid(currentField.uri.c_str(), "stored", value,
                    boolValue)) {
                currentField.stored = boolValue;
            }
        } else if (strcmp(name, "tokenized") == 0) {
            if (isBoolValid(currentField.uri.c_str(), "tokenized", value,
                    boolValue)) {
                currentField.tokenized = boolValue;
            }
        } else if (strcmp(name, "minCardinality") == 0) {
            currentField.min_cardinality = atoi(value);
        } else if (strcmp(name, "maxCardinality") == 0) {
            currentField.max_cardinality = atoi(value);
        }
    } else if (currentDefinition == defClass) {
        if (strcmp(name, "about") == 0) {
            warnIfLocale(val.c_str(), currentElementLang);
            if (currentClass.uri.size()) {
                cerr << "Uri is already defined for " << currentClass.uri
                    << "." << endl;
            }
        } else {
            currentClass.uri.assign(val);
        }
    } else if (strcmp(name, "label") == 0) {
        if (currentElementLang.size()) {
            ClassProperties::Localized l(
                currentClass.localized[currentElementLang]);
            if (l.name.size()) {
                cerr << "label[" << currentElementLang
                    << "] is already defined for " << currentClass.uri
                    << "." << endl;
            } else {
                l.name.assign(val);
                currentClass.localized[currentElementLang] = l;
            }
        } else if (currentClass.name.size()) {
            cerr << "label is already defined for " << currentClass.uri
                << "." << endl;
        } else {
            currentClass.name.assign(val);
        }
    } else if (strcmp(name, "comment") == 0) {
        if (currentElementLang.size()) {
            ClassProperties::Localized l(
                    currentClass.localized[currentElementLang]);
            if (l.description.size()) {
                cerr << "comment[" << currentElementLang
                    << "] is already defined for " << currentClass.uri
                    << "." << endl;
            } else {
                l.description.assign(val);
                currentClass.localized[currentElementLang] = l;
            }
        } else if (currentField.description.size()) {
            cerr << "comment is already defined for " << currentClass.uri
                << "." << endl;
        } else {
            currentClass.description.assign(val);
        }
    } else if(strcmp(name, "subClassOf") == 0) {
        replaceEntities(currentElementResource);
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
                p->setDefinitionAttribute((const char*)attributes[i*5],
                    (const char*)attributes[3+i*5]);
            }
        }
    } else {
        p->currentSubElement = (const char *)localname;
        for (int i=0; i<nb_attributes; i++) {
            if (strcmp((const char*)attributes[i*5], "resource") == 0) {
                p->currentElementResource = (const char*)attributes[3+i*5];
            } else if (strcmp((const char*)attributes[i*5], "lang") == 0) {
                p->currentElementLang = (const char*)attributes[3+i*5];
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
            if (p->currentField.uri.size()) {
                if(!p->currentField.alias.size()) {
                    size_t pos;
                    if( (pos = p->currentField.uri.rfind('#')) != string::npos) {
                        p->currentField.alias = p->currentField.uri.substr(pos+1);
                    }
                }
                p->pProperties[p->currentField.uri] = p->currentField;
                p->currentField.clear();
            }
            p->currentDefinition = defNone;
        } else if (strcmp((const char *)localname, "Class") == 0) {
            if (p->currentClass.uri.size()) {
                p->pClasses[p->currentClass.uri] = p->currentClass;
                p->currentClass.clear();
            }
            p->currentDefinition = defNone;
        } else {
            if (p->currentSubElement == (const char*)localname) {
                p->setDefinitionAttribute(p->currentSubElement.c_str(),
                p->currentElementChars.c_str());
                p->currentSubElement = "";
                p->currentElementChars = "";
                p->currentElementResource = "";
                p->currentElementLang = "";
            } else {
                cerr << "ERROR: Wrong closing element " << localname
                    << "." << endl;
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
FieldPropertiesDb::Private::warnIfLocale(const char* name, string& locale) {
    if (locale.size()) {
        cerr << "Warning: you cannot define a locale for the resource URI "
            << name << "." << endl;
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

#include "fieldpropertiesdb.h"
#include "fieldproperties.h"
#include <vector>
#include <sys/types.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#else
#include "stgdirent.h"
#endif
#include <sys/stat.h>
using namespace Strigi;
using namespace std;

class FieldPropertiesDb::Private {
public:
    map<string, FieldProperties> properties;
    static FieldProperties empty;

    Private();
    vector<string> getdirs(const string&) const;
    void loadProperties(const string& dir);
    void parseProperties(char*data);
    void handlePropertyLine(FieldProperties::Private& p, const char*name,
        const char* locale, const char*value);
    void storeProperties(FieldProperties::Private& props);
    void warnIfLocale(const char* name, const char* locale);
};
FieldProperties FieldPropertiesDb::Private::empty;

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
    return FieldPropertiesDb::Private::empty;
}
const map<string, FieldProperties>&
FieldPropertiesDb::allProperties() const {
    return p->properties;
}

FieldPropertiesDb::Private::Private() {
    const char* dirpath = getenv("XDG_DATA_HOME");
    vector<string> dirs;
    if (dirpath) {
        dirs = getdirs(dirpath);
    } else {
        dirpath = getenv("HOME");
        if (dirpath) {
             dirs = getdirs(string(dirpath)+"/.local/share");
        }
    }
    dirpath = getenv("XDG_DATA_DIRS");
    vector<string> d;
    if (dirpath) {
        d = getdirs(dirpath);
    } else {
        d = getdirs("/usr/local/share:/usr/share");
    }
    copy(d.begin(), d.end(), back_insert_iterator<vector<string> >(dirs));
    vector<string>::const_iterator i;
    for (i=dirs.begin(); i!=dirs.end(); i++) {
        loadProperties(*i);
    }
}
vector<string>
FieldPropertiesDb::Private::getdirs(const string& direnv) const {
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
void
FieldPropertiesDb::Private::loadProperties(const string& dir) {
    string pdir = dir + "/apps/strigi/fieldproperties/";
    DIR* d = opendir(pdir.c_str());
    if (!d) return;
    struct dirent* de = readdir(d);
    struct stat s;
    char* data = 0;
    while (de) {
        string path(pdir+de->d_name);
        if (!stat(path.c_str(), &s)) {
            if (S_ISREG(s.st_mode)) {
                FILE* f = fopen(path.c_str(), "r");
                if (f) {
                    data = (char*)realloc(data, s.st_size+1);
                    if (fread(data, 1, s.st_size, f) == (size_t)s.st_size) {
                        //fprintf(stderr, "parsing %s\n", path.c_str());
                        data[s.st_size] = '\0';
                        parseProperties(data);
                    }
                    fclose(f);
                }
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
    char* p = data;
    char* nl;
    do {
        nl = strchr(p, '\n');
        if (nl) *nl = '\0';
        if (*p == '#' || *p == '[') {
            storeProperties(props);
        }
        char* eq = strchr(p, '=');
        if (eq) {
            *eq = '\0';
            // find square brackets
            char* sbo = strchr(p, '[');
            if (sbo > eq) sbo = 0;
            char* sbc;
            if (sbo) {
                sbc = strchr(sbo, ']');
                if (sbc > eq || !sbc) {
                   sbo = 0;
                }
            }
            if (sbo) {
                *sbo = '\0';
                sbo++;
                *sbc = '\0';
            }
            handlePropertyLine(props, p, sbo, eq+1);
        }
        
        p = nl + 1;
    } while(nl);
    if (props.uri.size()) {
        properties[props.uri] = props;
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
FieldPropertiesDb::Private::handlePropertyLine(FieldProperties::Private& p,
        const char*name, const char* locale, const char*value) {
    if (strcmp(name, "Uri") == 0) {
        warnIfLocale(name, locale);
        if (p.uri.size()) {
            fprintf(stderr, "Uri is already defined for %s.\n", p.uri.c_str());
        } else {
            p.uri.assign(value);
        }
    } else if (strcmp(name, "TypeUri") == 0) {
        if (p.typeuri.size()) {
            fprintf(stderr, "TypeUri is defined twice for %s.\n",
                p.uri.c_str());
        } else {
            p.typeuri.assign(value);
        }
    } else if (strcmp(name, "Name") == 0) {
        if (locale) {
            FieldProperties::Localized l(p.localized[locale]);
            if (l.name.size()) {
                fprintf(stderr, "Name[%s] is already defined for %s.\n",
                     locale, p.uri.c_str());
            } else {
                l.name.assign(value);
                p.localized[locale] = l;
            }
        } else if (p.name.size()) {
            fprintf(stderr, "Name is already defined for %s.\n", p.uri.c_str());
        } else {
            p.name.assign(value);
        }
    } else if (strcmp(name, "Description") == 0) {
        if (locale) {
            FieldProperties::Localized l(p.localized[locale]);
            if (l.description.size()) {
                fprintf(stderr, "Description[%s] is already defined for %s.\n",
                     locale, p.uri.c_str());
            } else {
                l.description.assign(value);
                p.localized[locale] = l;
            }
        } else if (p.description.size()) {
            fprintf(stderr, "Description is already defined for %s.\n",
                p.uri.c_str());
        } else {
            p.description.assign(value);
        }
    } else if (strcmp(name, "ParentUri") == 0) {
        p.parentUris.push_back(value);
    }
}
void
FieldPropertiesDb::Private::warnIfLocale(const char* name, const char* locale) {
    if (locale) {
        fprintf(stderr, "Warning: you cannot define a locale for the field %s.",
            name);
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
FieldProperties::Private::clear() {
    uri.clear();
    name.clear();
    description.clear();
    localized.clear();
    typeuri.clear();
    parentUris.clear();
}

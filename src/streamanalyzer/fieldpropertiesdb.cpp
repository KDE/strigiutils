#include "fieldpropertiesdb.h"
#include "fieldproperties.h"
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
using namespace Strigi;
using namespace std;

class FieldPropertiesDb::Private {
public:
    Private();
    vector<string> getdirs() const;
    void loadProperties(const string& dir);
    void parseProperties(char*data);
    void handlePropertyLine(const char*name, const char* locale,
        const char*value);
};

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
const FieldProperties*
FieldPropertiesDb::getProperties(const std::string& uri) {
    return 0;
}

FieldPropertiesDb::Private::Private() {
    vector<string> dirs = getdirs();
    vector<string>::const_iterator i;
    fprintf(stderr, "dirs %i\n", dirs.size());
    for (i=dirs.begin(); i!=dirs.end(); i++) {
        fprintf(stderr, "dir %s\n", i->c_str());
        loadProperties(*i);
    }
}
vector<string>
FieldPropertiesDb::Private::getdirs() const {
    string direnv(getenv("KDEDIRS"));
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
    string pdir = dir + "/share/apps/strigi/fieldproperties/";
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
    FieldProperties props;
    char* p = data;
    char* nl;
    do {
        nl = strchr(p, '\n');
        if (nl) *nl = '\0';
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
            handlePropertyLine(p, sbo, eq+1);
        }
        
        p = nl + 1;
    } while(nl);

}
void
FieldPropertiesDb::Private::handlePropertyLine(const char*name,
        const char* locale, const char*value) {
    if (strcmp(name, "Type") == 0) {
    }
    fprintf(stderr, "%s\t%s\t%s\n", name, locale, value);
}

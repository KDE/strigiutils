#include "analyzerloader.h"
#include "analyzerfactoryfactory.h"
#include <string>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#define DLSYM dlsym
#define DLCLOSE dlclose
#else
#define DLSYM GetProcAddress
#define DLCLOSE FreeLibrary
#endif

using namespace std;
using namespace jstreams;

AnalyzerLoader::ModuleList AnalyzerLoader::modulelist;

AnalyzerLoader::ModuleList::ModuleList() {
}
AnalyzerLoader::ModuleList::~ModuleList() {
    map<string, Module*>::iterator i;
    for (i = modules.begin(); i != modules.end(); ++i) {
        delete i->second;
    }
}

AnalyzerLoader::Module::~Module() {
    void(*f)(const AnalyzerFactoryFactory*)
        = (void(*)(const AnalyzerFactoryFactory*))
        DLSYM(mod, "deleteStrigiAnalyzerFactory");
    if (f) {
        f(factory);
    }
    DLCLOSE(mod);
}
void
AnalyzerLoader::loadPlugins(const char* d) {
    DIR *dir = opendir(d);
    if (dir == 0) {
        // TODO handle error
        return;
    }
    struct dirent* ent = readdir(dir);
    while(ent) {
        size_t len = strlen(ent->d_name);
        if (strncmp(ent->d_name, "strigita_", 9) == 0
#ifdef WIN32
                && strcmp(ent->d_name+len-4, ".dll") == 0) {
#else
                && strcmp(ent->d_name+len-3, ".so") == 0) {
#endif
            string plugin = d;
            if (plugin[plugin.length()-1] != '/') {
                plugin.append("/");
            }
            plugin.append(ent->d_name);
            // check that the file is a regular file
            struct stat s;
            if (stat(plugin.c_str(), &s) == 0 && (S_IFREG & s.st_mode)) {
                printf("%s\n", plugin.c_str());
                loadModule(plugin.c_str());
            }
        }
        ent = readdir(dir);
    }
    closedir(dir);
}
void
AnalyzerLoader::loadModule(const char* lib) {
    if (AnalyzerLoader::modulelist.modules.find(lib)
            != AnalyzerLoader::modulelist.modules.end()) {
        // module was already loaded
        return;
    }
    StgModuleType handle;
#ifdef HAVE_DLFCN_H
    handle = dlopen(lib, RTLD_LAZY);
#else
    handle = LoadLibrary(lib);
#endif
    if (!handle) {
        return;
    }
    const AnalyzerFactoryFactory* (*f)() = (const AnalyzerFactoryFactory* (*)())
        DLSYM(handle, "getStrigiAnalyzerFactory");
    if (!f) {
        DLCLOSE(handle);
        return;
    }
    AnalyzerLoader::modulelist.modules[lib] = new Module(handle, f());
}
list<StreamEndAnalyzerFactory*>
AnalyzerLoader::getStreamEndAnalyzerFactories() {
    list<StreamEndAnalyzerFactory*> l;
    map<string, Module*>::iterator i;
    for (i = modulelist.modules.begin(); i != modulelist.modules.end(); ++i) {
        list<StreamEndAnalyzerFactory*> ml
            = i->second->factory->getStreamEndAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamThroughAnalyzerFactory*>
AnalyzerLoader::getStreamThroughAnalyzerFactories() {
    list<StreamThroughAnalyzerFactory*> l;
    map<string, Module*>::iterator i;
    for (i = modulelist.modules.begin(); i != modulelist.modules.end(); ++i) {
        list<StreamThroughAnalyzerFactory*> ml
            = i->second->factory->getStreamThroughAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}

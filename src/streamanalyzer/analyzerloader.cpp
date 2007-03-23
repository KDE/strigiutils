/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#include "analyzerloader.h"
#include "analyzerfactoryfactory.h"
#include <string>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <stgdirent.h>

#ifndef _WIN32
#include <dlfcn.h>
#define DLSYM dlsym
#define DLCLOSE dlclose
#else
#define DLSYM GetProcAddress
#define DLCLOSE FreeLibrary
#endif

#ifndef _WIN32
typedef void* StgModuleType;
#else
#include <windows.h>
typedef HMODULE StgModuleType;
#endif

using namespace std;
using namespace Strigi;

class AnalyzerLoader::Private {
public:
    class Module {
    private:
        const StgModuleType mod;
    public:
        Module(StgModuleType m, const AnalyzerFactoryFactory* f)
            :mod(m), factory(f) {}
        ~Module();
        const AnalyzerFactoryFactory* factory;
    };
    class ModuleList {
    public:
        ModuleList();
        ~ModuleList();
        std::map<std::string, Module*> modules;
    };
    static ModuleList modulelist;
    static void loadModule(const char* lib);
};

AnalyzerLoader::Private::ModuleList AnalyzerLoader::Private::modulelist;

AnalyzerLoader::Private::ModuleList::ModuleList() {
}
AnalyzerLoader::Private::ModuleList::~ModuleList() {
    map<string, Module*>::iterator i;
    for (i = modules.begin(); i != modules.end(); ++i) {
        delete i->second;
    }
}

AnalyzerLoader::Private::Module::~Module() {
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
        if ((strncmp(ent->d_name, "strigita_", 9) == 0
                || strncmp(ent->d_name, "strigiea_", 9) == 0)
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
                fprintf(stderr, "%s\n", plugin.c_str());
                Private::loadModule(plugin.c_str());
            }
        }
        ent = readdir(dir);
    }
    closedir(dir);
}
void
AnalyzerLoader::Private::loadModule(const char* lib) {
    if (AnalyzerLoader::Private::modulelist.modules.find(lib)
            != AnalyzerLoader::Private::modulelist.modules.end()) {
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
#ifndef WIN32
        fprintf(stderr, "%s\n", dlerror());
#else
        fprintf(stderr, "GetLastError: %d\n", GetLastError());
#endif
        DLCLOSE(handle);
        return;
    }
    AnalyzerLoader::Private::modulelist.modules[lib] = new Module(handle, f());
}
list<StreamEndAnalyzerFactory*>
AnalyzerLoader::getStreamEndAnalyzerFactories() {
    list<StreamEndAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamEndAnalyzerFactory*> ml
            = i->second->factory->getStreamEndAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamThroughAnalyzerFactory*>
AnalyzerLoader::getStreamThroughAnalyzerFactories() {
    list<StreamThroughAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamThroughAnalyzerFactory*> ml
            = i->second->factory->getStreamThroughAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamSaxAnalyzerFactory*>
AnalyzerLoader::getStreamSaxAnalyzerFactories() {
    list<StreamSaxAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamSaxAnalyzerFactory*> ml
            = i->second->factory->getStreamSaxAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamLineAnalyzerFactory*>
AnalyzerLoader::getStreamLineAnalyzerFactories() {
    list<StreamLineAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamLineAnalyzerFactory*> ml
            = i->second->factory->getStreamLineAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamEventAnalyzerFactory*>
AnalyzerLoader::getStreamEventAnalyzerFactories() {
    list<StreamEventAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamEventAnalyzerFactory*> ml
            = i->second->factory->getStreamEventAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
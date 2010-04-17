/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include <strigi/indexpluginloader.h>
#include <strigi/indexmanager.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stgdirent.h"
#include <sys/stat.h>
using namespace std;
using namespace Strigi;

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

vector<string> getdirs(const string& direnv) {
    vector<string> dirs;
    string::size_type lastp = 0;
    string::size_type p = direnv.find(PATH_SEPARATOR);
    while (p != string::npos) {
        dirs.push_back(direnv.substr(lastp, p-lastp));
        lastp = p+1;
        p = direnv.find(PATH_SEPARATOR, lastp);
    }
    dirs.push_back(direnv.substr(lastp));
    return dirs;
}

// anonymous namespace for static variables
namespace {
    class Module {
    private:
        const StgModuleType mod;
        Module(const Module&);
        void operator=(const Module&);
    public:
        Strigi::IndexManager* (*create)(const char*);
        void (*destroy)(Strigi::IndexManager*);
        Module(StgModuleType m)
            :mod(m) {}
        ~Module() {
// TODO: figure out why we get segfaults when cleaning up nicely
            DLCLOSE(mod);
        }
    };
    class ModuleList {
    private:
        map<std::string, Module*> modules;
    public:
        map<std::string, Module*>& mods() {
            if (!initialized) initialize();
            return modules;
        };
        map<void*, Module*> indexmanagers;
        bool initialized;

        ModuleList() {
            initialized = false;
        }
        void initialize() {
            initialized = true;
            // load the plugins from the environment setting
            string strigipluginpath;
            if (getenv("STRIGI_PLUGIN_PATH")) {
                strigipluginpath = getenv("STRIGI_PLUGIN_PATH");
            }
            vector<string> strigipluginpaths = getdirs(strigipluginpath);
            if (strigipluginpath.size()) {
                for (uint i=0; i<strigipluginpaths.size(); ++i) {
                    IndexPluginLoader::loadPlugins(strigipluginpaths[i].c_str());
                }
            } else {
                IndexPluginLoader::loadPlugins( LIBINSTALLDIR "/strigi");
            }
        }
        ~ModuleList() {
            // delete all leftover indexmanagers
            // if code deletes the indexmanager on it's own, the error will
            // appear here
            map<void*, Module*>::iterator j;
            for (j = indexmanagers.begin(); j != indexmanagers.end(); ++j) {
                j->second->destroy(static_cast<IndexManager*>(j->first));
            }
            // unload all the modules
            map<string, Module*>::iterator i;
            for (i = modules.begin(); i != modules.end(); ++i) {
                delete i->second;
            }
        }
        void loadModule(const string& name, const string& dir);
    };
    void
    ModuleList::loadModule(const string& name, const string& lib) {
        // check if this module was already loaded
        map<string, Module*>::iterator i = modules.find(name);
        if (i != modules.end()) {
            return;
        }
        StgModuleType handle;
#if defined(HAVE_DLFCN_H) && !defined(_WIN32)
        // do not use RTLD_GLOBAL here
        // note: If neither RTLD_GLOBAL nor RTLD_LOCAL are specified,
        // the default is RTLD_LOCAL.
        handle = dlopen(lib.c_str(), RTLD_LOCAL | RTLD_NOW);
#else
        handle = LoadLibrary(lib.c_str());
#endif
        if (!handle) {
#if defined(HAVE_DLFCN_H) && !defined(_WIN32)
            cerr << "Could not load '" << lib << "':" << dlerror() << endl;
#else
            cerr << "Could not load '" << lib << "': GetLastError(): "
                << GetLastError() << endl;
#endif
            return;
        }
        IndexManager*(*create)(const char*) = (IndexManager*(*)(const char*))
            DLSYM(handle, "createIndexManager");
        if (!create) {
#ifndef WIN32
            fprintf(stderr, "%s\n", dlerror());
#else
            fprintf(stderr, "GetLastError: %d\n", GetLastError());
#endif
            DLCLOSE(handle);
            return;
        }
        void(*destroy)(IndexManager*) = (void(*)(IndexManager*))
            DLSYM(handle, "deleteIndexManager");
        if (!destroy) {
#ifndef WIN32
            fprintf(stderr, "%s\n", dlerror());
#else
            fprintf(stderr, "GetLastError: %d\n", GetLastError());
#endif
            DLCLOSE(handle);
            return;
        }
        Module* module = new Module(handle);
        module->create = create;
        module->destroy = destroy;
        modules[name] = module;
    }
    static ModuleList modules;
}
void
IndexPluginLoader::loadPlugins(const char* d) {
    DIR *dir = opendir(d);
    if (dir == 0) {
        return;
    }
    struct dirent* ent = readdir(dir);
    string prefix("strigiindex_");
#ifdef WIN32
    string suffix(".dll");
#else
    string suffix(".so");
#endif
    while(ent) {
        size_t len = strlen(ent->d_name);
        const char* prepos = strstr(ent->d_name, prefix.c_str());
        const char* sufpos = strstr(ent->d_name, suffix.c_str());
        if (prepos && sufpos + suffix.length() == ent->d_name + len) {
            len -= (prepos - ent->d_name) + prefix.length() + suffix.length();
            string name(prepos + prefix.length(), len);
            string pluginpath = d;
            if (pluginpath[pluginpath.length()-1] != '/') {
                pluginpath.append("/");
            }
            pluginpath.append(ent->d_name);
            // check that the file is a regular file
            struct stat s;
            if (stat(pluginpath.c_str(), &s) == 0 && (S_IFREG & s.st_mode)) {
                modules.loadModule(name, pluginpath);
            }
        }
        ent = readdir(dir);
    }
    closedir(dir);
}
vector<string>
IndexPluginLoader::indexNames() {
    vector<string> names;
    map<string, Module*>::const_iterator i = modules.mods().begin();
    for (; i != modules.mods().end(); ++i) {
        names.push_back(i->first);
    }
    return names;
}
IndexManager*
IndexPluginLoader::createIndexManager(const char* name, const char* dir) {
    // find the right plugin
    map<string, Module*>::iterator i = modules.mods().find(name);
    if (i == modules.mods().end()) {
        return 0;
    }
    // create the indexmanager
    IndexManager* im = i->second->create(dir);
    if (im) {
        // map the indexmanager to the module that created it, so we can delete
        // it later
        modules.indexmanagers[im] = i->second;
    }
    return im;
}
void
IndexPluginLoader::deleteIndexManager(IndexManager* im) {
    // find the right module
    map<void*, Module*>::iterator i = modules.indexmanagers.find(im);
    if (i == modules.indexmanagers.end()) {
        return;
    }
    // let the module delete the indexmanager
    i->second->destroy(im);
    // remove the mapping from the map
    modules.indexmanagers.erase(i);
}

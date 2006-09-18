/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *                    Ben van Klinken <bvanklinken@gmail.com>
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
#include "jstreamsconfig.h"
#include "moduleloader.h"

#include <sys/types.h>
#include <sys/stat.h>
#include "stgdirent.h" //dirent replacement (includes native if available)
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_DLFCN_H
 #include <dlfcn.h>
#endif

using namespace std;
using namespace jstreams;

ModuleLoader::~ModuleLoader(){
    list<Module*>::iterator mod_itr = modules.begin();
    while ( mod_itr != modules.end() ){
       Module* mod = *mod_itr;
       delete mod;
       mod_itr++;
    }
    modules.clear();
}
void ModuleLoader::loadPlugins(const char* d) {
    DIR *dir = opendir(d);
    if (dir == 0) {
        // TODO handle error
        return;
    }
    struct dirent* ent = readdir(dir);
    while(ent) {
        //printf("%s\n", ent->d_name);
        size_t len = strlen(ent->d_name);
        if (strncmp(ent->d_name, "strigita_", 9) == 0
#ifdef _WIN32
                && strcmp(ent->d_name+len-4, ".dll") == 0) {
#else
                && strcmp(ent->d_name+len-3, ".so") == 0) {
#endif
            string plugin = d;
            if (plugin[plugin.length()-1] != '/') plugin += '/';
            plugin += ent->d_name;
            // check that the file is a regular file
            struct stat s;
            if (stat(plugin.c_str(), &s) == 0 && (S_IFREG & s.st_mode)) {
                Module* mod = loadModule(plugin.c_str());
                if ( mod ){
                    modules.push_back(mod);
                    fprintf(stderr, "loaded %s\n", plugin.c_str());
                }
            }
        }
        ent = readdir(dir);
    }
    closedir(dir);
}

ModuleLoader::Module* ModuleLoader::loadModule(const char* lib) {
    Module* mod = NULL;
   
#ifdef HAVE_DLFCN_H
    void* handle = dlopen (lib, RTLD_NOW);
    if (handle) {
        mod = new Module(handle);
    } else {
        fprintf(stderr, "%s\n", dlerror());
    }
#else
    HMODULE handle = LoadLibrary(lib);
    if (handle) {
        mod = new Module(handle);
    } else {
        LPVOID lpMsgBuf;
        DWORD dw = GetLastError();

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf, 0, NULL );

        wprintf(L"%s\n", lpMsgBuf);
        LocalFree(lpMsgBuf);
    }
    #endif
   
    if ( mod == NULL ) {
        return NULL;
    }
   
    mod->lib = lib;
    if ( !mod->init() ) {
        delete mod;   
        return NULL;
    } else {
        return mod;
    }
}
void
ModuleLoader::getThroughAnalyzers(
        multimap<void*, StreamThroughAnalyzer*>& analyzers){
    list<Module*>::iterator mod_itr = modules.begin();
    while ( mod_itr != modules.end() ){
       Module* mod = *mod_itr;
       if ( mod->createThroughFunc ){
           StreamThroughAnalyzer* a = NULL;
           int i = 0;
           while (mod->createThroughFunc( i, &a )) {
                if (a) {
                    analyzers.insert(make_pair(mod, a));
                }
                ++i;
            }
            if (analyzers.size() == 0) {
                fprintf(stderr,
                    "Warning: doesn't contain any through analyzers\n");
            }
        }
        mod_itr++;
    }
}
void
ModuleLoader::getEndAnalyzers(multimap<void*, StreamEndAnalyzer*>& analyzers){
    list<Module*>::iterator mod_itr = modules.begin();
    while ( mod_itr != modules.end() ){
        Module* mod = *mod_itr;
        if ( mod->createEndFunc ){
            StreamEndAnalyzer* a = NULL;
            int i=0;
            while (mod->createEndFunc(i, &a)) {
                if (a) {
                    analyzers.insert(make_pair(mod, a));
                }
                ++i;
            }
            if (analyzers.size() == 0) {
                fprintf(stderr, "Warning: doesn't contain any end analyzers\n");
            }
        }
        mod_itr++;
    }
}
void
ModuleLoader::deleteThroughAnalyzers(multimap<void*, StreamThroughAnalyzer*>&
        analyzers) {
    multimap<void*, StreamThroughAnalyzer*>::const_iterator i;
    for (i = analyzers.begin(); i != analyzers.end(); i++) {
        Module* mod = static_cast<Module*>(i->first);
        if ( mod->deleteFunc ) {
            mod->deleteFunc(i->second);
        }
    }
}
void
ModuleLoader::deleteEndAnalyzers(multimap<void*, StreamEndAnalyzer*>&
        analyzers){
    multimap<void*, StreamEndAnalyzer*>::const_iterator i;
    for (i = analyzers.begin(); i != analyzers.end(); i++) {
        Module* mod = static_cast<Module*>(i->first);
        if ( mod->deleteFunc ) {
            mod->deleteFunc(i->second);
        }
    }
}

ModuleLoader::Module::Module(StgModuleType hmodule){
    this->handle = hmodule;
    this->createThroughFunc = NULL;
    this->createEndFunc = NULL;
    this->deleteFunc = NULL;
}
ModuleLoader::Module::~Module(){
#ifdef HAVE_DLFCN_H
    dlclose(handle);
#else
    FreeLibrary(handle);
#endif
}
bool
ModuleLoader::Module::init(){
#ifdef HAVE_DLFCN_H
    createThroughFunc = (createThroughAnalyzer_t*)dlsym(handle,
        "createThroughAnalyzer");
    createEndFunc = (createEndAnalyzer_t*)dlsym(handle, "createEndAnalyzer");
    deleteFunc = (deleteAnalyzer_t*)dlsym(handle, "deleteAnalyzer");
#else
    createThroughFunc = (createThroughAnalyzer_t*)GetProcAddress(handle, "createThroughAnalyzer");
    createEndFunc = (createEndAnalyzer_t*)GetProcAddress(handle, "createEndAnalyzer");
    deleteFunc = (deleteAnalyzer_t*)GetProcAddress(handle, "deleteAnalyzer");
#endif

    if ( !createThroughFunc && !createEndFunc ){
        fprintf(stderr, "%s not a valid strigi plugin\n", lib.c_str());
        return false;
    }
    if ( !deleteFunc ){
        fprintf(stderr, "Warning: %s does not have a deleteAnalyzer function. "
            "Memory leaks will occur\n", lib.c_str());
    }
    return true;
}

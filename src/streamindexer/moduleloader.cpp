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

ModuleLoader::~ModuleLoader(){
    std::list<Module*>::iterator mod_itr = modules.begin();
    while ( mod_itr != modules.end() ){
       Module* mod = *mod_itr;
       delete mod;
       mod_itr++;
    }
    modules.clear();
}
void ModuleLoader::loadPlugins(const char*d) {
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
                Module* mod = ModuleLoader::loadModule(plugin.c_str());
                if ( mod ){
                    modules.push_back(mod);
                    printf("loaded %s\n", plugin.c_str());
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
        printf("%s\n", dlerror());
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
ModuleLoader::getThroughAnalyzers(std::list<ThroughPair>* analyzers){
    std::list<Module*>::iterator mod_itr = modules.begin();
    while ( mod_itr != modules.end() ){
       Module* mod = *mod_itr;
       if ( mod->createThroughFunc ){
           int i=0;
           do{
                jstreams::StreamThroughAnalyzer* a = NULL;
                if ( !mod->createThroughFunc( i, &a ) ){
                    if ( i==0 )
                        printf("Warning: doesn't contain any through analyzers\n");
                    break;
                }else if ( a != NULL ){
                    ThroughPair pair = {a,mod};
                    analyzers->push_back( pair );
                }
                ++i;
            }while(true);
        }
       mod_itr++;
    }
}

void ModuleLoader::getEndAnalyzers(std::list<EndPair>* analyzers){
    std::list<Module*>::iterator mod_itr = modules.begin();
    while ( mod_itr != modules.end() ){
       Module* mod = *mod_itr;
       if ( mod->createEndFunc ){
           int i=0;
           do{
                jstreams::StreamEndAnalyzer* a = NULL;
                if ( !mod->createEndFunc(i, &a) ){
                    if ( i==0 )
                        printf("Warning: doesn't contain any end analyzers\n");
                    break;
                }else if ( a != NULL ){
                    EndPair pair = {a,mod};
                    analyzers->push_back( pair );
                }
                ++i;
            }while(true);
        }
       mod_itr++;
    }
}

void
ModuleLoader::deleteThroughAnalyzers(std::list<ThroughPair>* analyzers){
    std::list<ThroughPair>::iterator itr = analyzers->begin();
    while ( itr != analyzers->end() ){
        Module* mod = itr->mod;
        if ( mod->deleteFunc )
            mod->deleteFunc(itr->analyzer);
        ++itr;
    }
}
void
ModuleLoader::deleteEndAnalyzers(std::list<EndPair>* analyzers){
    std::list<EndPair>::iterator itr = analyzers->begin();
    while ( itr != analyzers->end() ){
        Module* mod = itr->mod;
        if ( mod->deleteFunc )
            mod->deleteFunc(itr->analyzer);
        ++itr;
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
    createThroughFunc = (createThroughAnalyzer*)GetProcAddress(handle, "createThroughAnalyzer");
    createEndFunc = (createEndAnalyzer*)GetProcAddress(handle, "createEndAnalyzer");
    deleteFunc = (deleteAnalyzer*)GetProcAddress(handle, "deleteAnalyzer");
#endif

    if ( !createThroughFunc && !createEndFunc ){
        printf("%s not a valid strigi plugin\n", lib.c_str());
        return false;
    }
    if ( !deleteFunc ){
        printf("Warning: %s doesnt have a deleteAnalyzer function. Memory leaks will occur\n", lib.c_str());
    }
    return true;
}

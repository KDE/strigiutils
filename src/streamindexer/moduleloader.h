#ifndef MODULELOADER_H
#define MODULELOADER_H
#include "jstreamsconfig.h"
#include "streamthroughanalyzer.h"
#include "streamendanalyzer.h"
#include <list>

#ifdef HAVE_DLFCN_H
 typedef void* StgModuleType;
#else
 typedef HMODULE StgModuleType;
#endif

class ModuleLoader{
public:
    //typedefs for entry points into strigi plugins
    typedef bool (createEndAnalyzer_t)(int item, jstreams::StreamEndAnalyzer**);
    typedef bool (createThroughAnalyzer_t)(int item, jstreams::StreamThroughAnalyzer**);
    typedef void (deleteAnalyzer_t)(void* analyzer);
private:
    class Module{
    public:
        //pointers to plugin entry points. this saves dlsym calls
        createThroughAnalyzer_t* createThroughFunc;
        createEndAnalyzer_t* createEndFunc;
        deleteAnalyzer_t* deleteFunc;
        StgModuleType handle;

        Module(StgModuleType hmodule);
        ~Module();
        std::string lib; //for information purpose only

        //try to initialise the plugin, if this returns false, then the module loading failed.
        bool init();
    };
    std::list<Module*> modules; //list of all available modules
public:
    ~ModuleLoader();

    //the get*Analyzer functions returns a list of analyzer/module structs. these
    //structs are required so that a pairing of analyzer & module can be maintained
    //so that later the analyzer can be deleted using the same module that it was created with
    struct ThroughPair{
        jstreams::StreamThroughAnalyzer* analyzer;
        Module* mod;
    };
    struct EndPair{
        jstreams::StreamEndAnalyzer* analyzer;
        Module* mod;
    };

    //goes retrieves all the modules in this moduleloader add adds the through analyzers
    //for each module to the list
    void getThroughAnalyzers(std::list<ThroughPair>* analyzers);
    //deletes each analyzer in the list.
    static void deleteThroughAnalyzers(std::list<ThroughPair>* analyzers);
    
    //goes retrieves all the modules in this moduleloader add adds the end analyzers
    //for each module to the list
    void getEndAnalyzers(std::list<EndPair>* analyzers);
    //deletes each analyzer in the list.
    static void deleteEndAnalyzers(std::list<EndPair>* analyzers);

    //finds and loads all strigi plugins from the given directory into this moduleloader
    void ModuleLoader::loadPlugins(const char* d);

    //load a given module from the specified file path
    static Module* loadModule(const char* lib);
public:
    
};
#endif //MODULELOADER_H

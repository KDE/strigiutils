#ifndef MODULELOADER_H
#define MODULELOADER_H
#include "jstreamsconfig.h"

#include <string>
#include <list>
#include <map>

#ifdef HAVE_DLFCN_H
 typedef void* StgModuleType;
#else
 typedef HMODULE StgModuleType;
#endif

namespace jstreams {
    class StreamThroughAnalyzer;
    class StreamEndAnalyzer;
}

class ModuleLoader{
public:
    //typedefs for entry points into strigi plugins
    typedef bool (createEndAnalyzer_t)(int item, jstreams::StreamEndAnalyzer**);
    typedef bool (createThroughAnalyzer_t)(int item, jstreams::StreamThroughAnalyzer**);
    typedef void (deleteAnalyzer_t)(void* analyzer);
private:
    class Module {
    public:
        //pointers to plugin entry points. this saves dlsym calls
        createThroughAnalyzer_t* createThroughFunc;
        createEndAnalyzer_t* createEndFunc;
        deleteAnalyzer_t* deleteFunc;
        StgModuleType handle;

        Module(StgModuleType hmodule);
        ~Module();
        std::string lib; //for information purpose only

        /* try to initialise the plugin, if this returns false, then the module
         * loading failed.
         */
        bool init();
    };
    std::list<Module*> modules; //list of all available modules
public:
    ~ModuleLoader();

    /* The get*Analyzer functions returns a list of analyzer/module structs.
     * These structs are required so that a pairing of analyzer & module can
     * be maintained so that later the analyzer can be deleted using the same
     * module that it was created with.
     */

    /* Retrieves all the modules in this moduleloader add adds the through
     * analyzers for each module to the list.
     */
    void getThroughAnalyzers(std::multimap<void*,
        jstreams::StreamThroughAnalyzer*>& analyzers);
    //deletes each analyzer in the list.
    static void deleteThroughAnalyzers(std::multimap<void*,
        jstreams::StreamThroughAnalyzer*>& analyzers);
 
    /* retrieves all the modules in this moduleloader add adds the end
     * analyzers for each module to the list
     */
    void getEndAnalyzers(std::multimap<void*,
        jstreams::StreamEndAnalyzer*>& analyzers);
    //deletes each analyzer in the list.
    static void deleteEndAnalyzers(std::multimap<void*,
        jstreams::StreamEndAnalyzer*>& analyzers);

    //finds and loads all strigi plugins from the given directory into this moduleloader
    void loadPlugins(const char* d);

    //load a given module from the specified file path
    static Module* loadModule(const char* lib);
public:
    
};
#endif //MODULELOADER_H

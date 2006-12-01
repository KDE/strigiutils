#include "jstreamsconfig.h"
#include <list>
#include <map>

#ifdef HAVE_DLFCN_H
 typedef void* StgModuleType;
#else
 typedef HMODULE StgModuleType;
#endif

namespace jstreams {
class AnalyzerFactoryFactory;
class StreamEndAnalyzerFactory;
class StreamThroughAnalyzerFactory;
class AnalyzerLoader {
private:
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
public:
    static void loadPlugins(const char* dir);
    std::list<StreamEndAnalyzerFactory*> getStreamEndAnalyzerFactories();
    std::list<StreamThroughAnalyzerFactory*>
        getStreamThroughAnalyzerFactories();
};
}

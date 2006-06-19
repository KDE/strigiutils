#ifndef PLUGINTHROUGHANALYZER_H
#define PLUGINTHROUGHANALYZER_H

#include <vector>
#include <list>
#include "streamthroughanalyzer.h"
#include "indexwriter.h"

typedef jstreams::StreamThroughAnalyzer* (*createThroughAnalyzer_t)();
typedef void (*destroyThroughAnalyzer_t)(
    const jstreams::StreamThroughAnalyzer*);

class PluginThroughAnalyzer : public jstreams::StreamThroughAnalyzer {
public:
    class Factory {
    private:
        Factory(const Factory& f);
        void operator=(const Factory& f);
    public:
        const createThroughAnalyzer_t create;
        const destroyThroughAnalyzer_t destroy;

        Factory(createThroughAnalyzer_t c, destroyThroughAnalyzer_t d);
        ~Factory();
    };
    class FactoryCleaner {
    private:
        std::list<void*> loadedModules;
    public:
        ~FactoryCleaner();
        void addModule(void*h) {
            loadedModules.push_back(h);
        }
    };
private:
    static std::list<const Factory*> factories;
    std::vector<jstreams::StreamThroughAnalyzer*> analyzers;

    static FactoryCleaner cleaner;

    static void loadPlugin(const std::string& lib);

public:
    static void loadPlugins(const char* dir);

    PluginThroughAnalyzer();
    ~PluginThroughAnalyzer();
    void setIndexable(jstreams::Indexable* i);
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
};

// macro for registering a ThroughAnalyzer in a module
#define REGISTER_THROUGHANALYZER(CLASS) \
jstreams::StreamThroughAnalyzer* create ## CLASS() { \
    return new CLASS(); \
} \
void destroy ## CLASS(const jstreams::StreamThroughAnalyzer* a) { \
    delete a; \
} \
extern "C" { \
PluginThroughAnalyzer::Factory CLASS ## Factory(create ## CLASS, \
    destroy ## CLASS); \
}

#endif

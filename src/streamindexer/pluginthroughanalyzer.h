#ifndef PLUGINTHROUGHANALYZER_H
#define PLUGINTHROUGHANALYZER_H

#include <vector>
#include <list>
#include "streamthroughanalyzer.h"

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
        createThroughAnalyzer_t* create;
        const destroyThroughAnalyzer_t destroy;

        Factory(createThroughAnalyzer_t* c, destroyThroughAnalyzer_t d);
        ~Factory();
    };
private:
/*    struct Plugin {
        std::string pluginfile;
        lt_dlhandle handle;
        Factory* factory;
        std::vector<jstreams::StreamThroughAnalyzer*> analyzers;
    };

    std::vector<Plugin> plugins;*/
    static std::list<const Factory*> factories;
    std::vector<jstreams::StreamThroughAnalyzer*> analyzers;

    static void loadPlugin(const std::string& lib);
//    void destroyAnalyzers(const Plugin& plugin);
//    void unloadPlugin(const Plugin& plugin);

public:
    static void loadPlugins(const char* dir);

    PluginThroughAnalyzer();
    ~PluginThroughAnalyzer();
    void setIndexable(jstreams::Indexable* i);
    jstreams::InputStream *connectInputStream(jstreams::InputStream *in);
};

#endif

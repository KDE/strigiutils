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

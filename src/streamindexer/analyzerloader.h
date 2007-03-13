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
#include "jstreamsconfig.h"
#include <list>
#include <map>
#include <string>

#ifndef _WIN32
typedef void* StgModuleType;
#else
#include <windows.h>
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

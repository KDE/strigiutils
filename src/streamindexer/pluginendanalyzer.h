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
#ifndef PluginEndAnalyzer_H
#define PluginEndAnalyzer_H

#include "streamendanalyzer.h"
#include <map>

class ModuleLoader;
class PluginEndAnalyzer : public jstreams::StreamEndAnalyzer {
public:
private:
    //a map of analyzers that this plugin has created, and their corresponding modules
    //for cleanup purposes.
    std::multimap<void*, jstreams::StreamEndAnalyzer*> analyzers;
    ModuleLoader* moduleLoader;

    mutable jstreams::StreamEndAnalyzer* selectedEndAnalyzer;
public:
    explicit PluginEndAnalyzer(ModuleLoader* moduleLoader);
    ~PluginEndAnalyzer();
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(std::string filename, jstreams::InputStream *in, int depth,
        jstreams::StreamIndexer *indexer, jstreams::Indexable*);
    const char* getName() const { return "PluginEndAnalyzer"; }
    static void loadPlugins(const char* dir);
};


#endif

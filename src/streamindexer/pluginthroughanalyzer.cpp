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
#include "pluginthroughanalyzer.h"
#include "moduleloader.h"

using namespace std;
using namespace jstreams;

PluginThroughAnalyzer::PluginThroughAnalyzer(ModuleLoader* ml) {
    moduleLoader = ml;
   
    /* For now we just load all the plugins in any order...
     * configuration required
     */
    moduleLoader->getThroughAnalyzers(analyzers);
}
PluginThroughAnalyzer::~PluginThroughAnalyzer() {
    moduleLoader->deleteThroughAnalyzers(analyzers);
}
InputStream *
PluginThroughAnalyzer::connectInputStream(InputStream *in) {
    multimap<void*, StreamThroughAnalyzer*>::const_iterator i;
    for (i = analyzers.begin(); i!= analyzers.end(); ++i) {
        in = (i->second)->connectInputStream(in);
    }
    return in;
}
void
PluginThroughAnalyzer::setIndexable(Indexable* idx) {
    multimap<void*, StreamThroughAnalyzer*>::const_iterator i;
    for (i = analyzers.begin(); i!= analyzers.end(); ++i) {
        StreamThroughAnalyzer* a = i->second;
        a->setIndexable(idx);
    }
}
bool
PluginThroughAnalyzer::isReadyWithStream() {
    bool ready = true;
    multimap<void*, StreamThroughAnalyzer*>::const_iterator i;
    for (i = analyzers.begin(); ready && i != analyzers.end(); ++i) {
        ready = i->second->isReadyWithStream();
    }
    return ready;
}

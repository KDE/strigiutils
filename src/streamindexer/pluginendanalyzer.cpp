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
#include "pluginendanalyzer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "stgdirent.h" //dirent replacement (includes native if available)
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#ifdef HAVE_DLFCN_H
 #include <dlfcn.h>
#else
#endif

using namespace std;
using namespace jstreams;


PluginEndAnalyzer::PluginEndAnalyzer(ModuleLoader* ml) {
	moduleLoader = ml;
	selectedEndAnalyzer = NULL;
	
	//for now we just load all the plugins in any order... configuration required
    moduleLoader->getEndAnalyzers(&analyzers);
}
PluginEndAnalyzer::~PluginEndAnalyzer() {
	moduleLoader->deleteEndAnalyzers(&analyzers);
}

bool PluginEndAnalyzer::checkHeader(const char* header, int32_t headersize) const{
    std::list<ModuleLoader::EndPair>::const_iterator i;
    for (i = analyzers.begin(); i!= analyzers.end(); ++i) {
        if ( (i->analyzer)->checkHeader(header, headersize) ){
        	selectedEndAnalyzer = i->analyzer;
        	return true;
        }
    }
    return false;
}
char PluginEndAnalyzer::analyze(std::string filename, jstreams::InputStream *in, int depth,
        jstreams::StreamIndexer *indexer, jstreams::Indexable* idx) {

	if ( selectedEndAnalyzer ){
    	char ret = selectedEndAnalyzer->analyze(filename,in,depth,indexer, idx);
		selectedEndAnalyzer = NULL;
		return ret;
    }else
    	return -1;
}


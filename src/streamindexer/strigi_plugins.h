/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *                    Ben van Klinken <bvanklinken@gmail.com>
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
#ifndef STRIGI_PLUGINS_H
#define STRIGI_PLUGINS_H

#include "streamthroughanalyzer.h"
#include "streamendanalyzer.h"
#include <vector>
#include <list>

class ThroughAnalyzerFactory {
public:
    virtual ~ThroughAnalyzerFactory() {}
    virtual jstreams::StreamThroughAnalyzer* create() = 0;
    virtual std::list<std::string> getFieldNames() {
        return std::list<std::string>();
    }
};

template<typename CLASS>
class ThroughAnalyzerFactoryImpl: public ThroughAnalyzerFactory{
public:
    jstreams::StreamThroughAnalyzer* create(){
        return new CLASS;
    }
};
std::vector<ThroughAnalyzerFactory*> strigi_through_analyzer_factories;

#ifdef STRIGI_IMPORT_API
 #ifdef WIN32
  #define STRIGI_PLUGIN_API __declspec(dllexport)
 #else
  #define STRIGI_PLUGIN_API
 #endif
#else
  #error "You should add STRIGI_IMPORT_API to the top of you plugin, you should not include this header if your code is not a plugin"
#endif


  // macro for registering a ThroughAnalyzer in a module
#define STRIGI_THROUGH_PLUGINS_START \
extern "C" { \
    STRIGI_PLUGIN_API bool createThroughAnalyzer(uint item, \
            jstreams::StreamThroughAnalyzer** ret) { \
        if (strigi_through_analyzer_factories.size() == 0) {

#define STRIGI_THROUGH_PLUGINS_REGISTER(CLASS) \
            strigi_through_analyzer_factories.push_back( \
                (ThroughAnalyzerFactory*)new ThroughAnalyzerFactoryImpl<CLASS>);

#define STRIGI_THROUGH_PLUGINS_END \
        } \
        if ( item < 0 || item >= strigi_through_analyzer_factories.size() ) { \
            return false; \
        } \
        *ret = strigi_through_analyzer_factories.at(item)->create(); \
        return true; \
    } \
    STRIGI_PLUGIN_API void deleteAnalyzer( \
            jstreams::StreamThroughAnalyzer* analyzer) { \
        delete analyzer; \
    } \
}


class EndAnalyzerFactory{
public:
    virtual ~EndAnalyzerFactory() {}
    virtual jstreams::StreamEndAnalyzer* create() = 0;
};
template<typename CLASS>
class EndAnalyzerFactoryImpl: public EndAnalyzerFactory{
public:
    jstreams::StreamEndAnalyzer* create(){
        return new CLASS;
    }
    virtual std::list<std::string> getFieldNames() {
        return std::list<std::string>();
    }
    virtual bool readsSubStreams() const {
        return false;
    }
};
std::vector<EndAnalyzerFactory*> strigi_end_analyzer_factories;

#ifdef STRIGI_IMPORT_API
 #ifdef WIN32
  #define STRIGI_PLUGIN_API __declspec(dllexport)
 #else
  #define STRIGI_PLUGIN_API
 #endif
#else
  #error "You should add STRIGI_IMPORT_API to the top of you plugin, you should not include this header if your code is not a plugin"
#endif


  // macro for registering a EndAnalyzer in a module
#define STRIGI_END_PLUGINS_START extern "C"{ \
    STRIGI_PLUGIN_API bool createEndAnalyzer(int item, jstreams::StreamEndAnalyzer** ret){ \
        if ( strigi_end_analyzer_factories.size() == 0 ){
#define STRIGI_END_PLUGINS_REGISTER(CLASS) strigi_end_analyzer_factories.push_back((EndAnalyzerFactory*)new EndAnalyzerFactoryImpl<CLASS>);
#define STRIGI_END_PLUGINS_END \
        } \
        if ( item < 0 || item >= strigi_end_analyzer_factories.size() ) return false; \
        *ret = strigi_end_analyzer_factories.at(item)->create(); return true;\
    } \
    STRIGI_PLUGIN_API void deleteAnalyzer(jstreams::StreamEndAnalyzer* analyzer){ delete analyzer; } \
}

#include "indexwriter.h"

#endif //STRIGI_PLUGINS_H

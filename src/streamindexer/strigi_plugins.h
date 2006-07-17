#ifndef STRIGI_PLUGINS_H
#define STRIGI_PLUGINS_H

#include "streamthroughanalyzer.h"
#include "streamendanalyzer.h"
#include <vector>

class ThroughAnalyzerFactory {
public:
    virtual jstreams::StreamThroughAnalyzer* create() = 0;
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
    STRIGI_PLUGIN_API bool createThroughAnalyzer(int item, \
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
    virtual jstreams::StreamEndAnalyzer* create() = 0;
};
template<typename CLASS>
class EndAnalyzerFactoryImpl: public EndAnalyzerFactory{
public:
    jstreams::StreamEndAnalyzer* create(){
        return new CLASS;
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
#define STRIGI_END_PLUGINS_START() extern "C"{ \
    STRIGI_PLUGIN_API bool createEndAnalyzer(int item, jstreams::StreamEndAnalyzer** ret){ \
        if ( strigi_end_analyzer_factories.size() == 0 ){
#define STRIGI_END_PLUGINS_REGISTER(CLASS) strigi_end_analyzer_factories.push_back((EndAnalyzerFactory*)new EndAnalyzerFactoryImpl<CLASS>);
#define STRIGI_END_PLUGINS_END() \
        } \
        if ( item < 0 || item >= strigi_end_analyzer_factories.size() ) return false; \
        *ret = strigi_end_analyzer_factories.at(item)->create(); return true;\
    } \
    STRIGI_PLUGIN_API void deleteAnalyzer(void* analyzer){ delete analyzer; } \
}


#endif //STRIGI_PLUGINS_H

#include "analyzerfactoryfactory.h"

#ifdef STRIGI_IMPORT_API
 #ifdef WIN32
  #define STRIGI_PLUGIN_API __declspec(dllexport)
 #else
  #define STRIGI_PLUGIN_API
 #endif
#else
  #error "You should add STRIGI_IMPORT_API to the top of you plugin, you should not include this header if your code is not a plugin"
#endif
#define STRIGI_ANALYZER_FACTORY(CLASS) extern "C" { \
    const AnalyzerFactoryFactory* getStrigiAnalyzerFactory() { \
        return new CLASS(); \
    } \
    void deleteStrigiAnalyzerFactory(const AnalyzerFactoryFactory* f) { \
        delete f; \
    } \
}

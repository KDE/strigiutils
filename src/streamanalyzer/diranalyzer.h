#ifndef DIRANALYZER_H
#define DIRANALYZER_H

#include "strigiconfig.h"
#include <string>

namespace Strigi {
class IndexManager;
class AnalyzerConfiguration;
class STREAMANALYZER_EXPORT DirAnalyzer {
public:
    class Private;
private:
    Private* p;
public:
    DirAnalyzer(IndexManager& manager, AnalyzerConfiguration& conf);
    ~DirAnalyzer();
    int analyzeDir(const std::string& dir, int nthreads = 2);
    int updateDir(const std::string& dir, int nthreads = 2);
};
}
#endif

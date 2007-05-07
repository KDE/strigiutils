#ifndef DIRANALYZER_H
#define DIRANALYZER_H

#include "strigiconfig.h"
#include <string>
#include <vector>

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
    int analyzeDir(const std::string& dir, int nthreads = 2,
        bool (*continueAnalysis)() = 0);
    int updateDir(const std::string& dir, int nthreads = 2,
        bool (*continueAnalysis)() = 0);
    int updateDirs(const std::vector<std::string>& dirs, int nthreads = 2,
        bool (*continueAnalysis)() = 0);
};
}
#endif

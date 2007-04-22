#ifndef DIRANALYZER_H
#define DIRANALYZER_H

#include "strigiconfig.h"
#include <string>

namespace Strigi {
class IndexWriter;
class AnalyzerConfiguration;
class STREAMANALYZER_EXPORT DirAnalyzer {
public:
    class Private;
private:
    Private* p;
public:
   DirAnalyzer(IndexWriter& writer, AnalyzerConfiguration* conf = 0);
   ~DirAnalyzer();
   int analyzeDir(const std::string& dir, int nthreads = 1);
};
}
#endif

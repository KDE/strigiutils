#ifndef IFILTERENDANALYZER
#define IFILTERENDANALYZER

#include "streamendanalyzer.h"
#include <set>
#include <iconv.h>

class IFilterEndAnalyzer : public jstreams::StreamEndAnalyzer {
    static long initd;
	
    std::string writeToTempFile(jstreams::InputStream *in, const char* ext) const;
    bool checkForFile(int depth, const std::string& filename);
	static std::set<std::string> extensions;
	iconv_t converter;
public:
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(jstreams::Indexable& idx, jstreams::InputStream *in);
    const char* getName() const { return "IFilterEndAnalyzer"; }
    
    IFilterEndAnalyzer();
    ~IFilterEndAnalyzer();
};

#endif

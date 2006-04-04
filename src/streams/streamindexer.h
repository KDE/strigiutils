#ifndef STREAMINDEXER_H
#define STREAMINDEXER_H

#include <vector>
#include <string>

class InputStream;
class StreamEndAnalyzer;
class StreamThroughAnalyzer;

class StreamIndexer {
private:
    std::vector<std::vector<StreamEndAnalyzer*> > end;
    std::vector<std::vector<StreamThroughAnalyzer*> > through;

    void addThroughAnalyzers();
    void addEndAnalyzers();
public:
    StreamIndexer();
    ~StreamIndexer();
    char indexFile(const char *filepath);
    char analyze(std::string &path, InputStream *input, uint depth);
};

#endif

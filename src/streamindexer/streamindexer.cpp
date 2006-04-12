#include "streamindexer.h"
#include "fileinputstream.h"
#include "streamthroughanalyzer.h"
#include "bz2endanalyzer.h"
#include "tarendanalyzer.h"
#include "digestthroughanalyzer.h"
using namespace jstreams;

StreamIndexer::StreamIndexer() {
}
StreamIndexer::~StreamIndexer() {
    // delete the through analyzers and end analyzers
    std::vector<std::vector<StreamThroughAnalyzer*> >::iterator tIter;
    for (tIter = through.begin(); tIter != through.end(); ++tIter) {
        std::vector<StreamThroughAnalyzer*>::iterator t;
        for (t = tIter->begin(); t != tIter->end(); ++t) {
            delete *t;
        }
    }
    std::vector<std::vector<StreamEndAnalyzer*> >::iterator eIter;
    for (eIter = end.begin(); eIter != end.end(); ++eIter) {
        std::vector<StreamEndAnalyzer*>::iterator e;
        for (e = eIter->begin(); e != eIter->end(); ++e) {
            delete *e;
        }
    }
}
char
StreamIndexer::indexFile(const char *filepath) {
    std::string path(filepath);
    FileInputStream file(filepath);
    return analyze(path, &file, 0);
}
void
StreamIndexer::addThroughAnalyzers() {
    through.resize(through.size()+1);
    std::vector<std::vector<StreamThroughAnalyzer*> >::reverse_iterator tIter;
    tIter = through.rbegin();
    StreamThroughAnalyzer* ana = new DigestThroughAnalyzer();
    tIter->push_back(ana);
}
void
StreamIndexer::addEndAnalyzers() {
    end.resize(end.size()+1);
    std::vector<std::vector<StreamEndAnalyzer*> >::reverse_iterator eIter;
    eIter = end.rbegin();
    StreamEndAnalyzer* ana = new BZ2EndAnalyzer();
    eIter->push_back(ana);
    ana = new TarEndAnalyzer();
    eIter->push_back(ana);
}
char
StreamIndexer::analyze(std::string &path, InputStream *input, uint depth) {
    // retrieve or construct the through analyzers and end analyzers
    std::vector<std::vector<StreamThroughAnalyzer*> >::iterator tIter;
    std::vector<std::vector<StreamEndAnalyzer*> >::iterator eIter;
    while (through.size() < depth+1) {
        addThroughAnalyzers();
        addEndAnalyzers();
    }
    tIter = through.begin() + depth;
    eIter = end.begin() + depth;

    // insert the through analyzers
    std::vector<StreamThroughAnalyzer*>::iterator ts;
    for (ts = tIter->begin(); ts != tIter->end(); ++ts) {
        input = (*ts)->connectInputStream(input);
    }

    bool finished = false;
    input->mark(1024); // set to size required to determine file type
    std::vector<StreamEndAnalyzer*>::iterator es = eIter->begin();
    while (!finished && es != eIter->end()) {
        char r = (*es)->analyze(path, input, depth+1, this);
        if (r) {
            StreamStatus ir = input->reset();
            if (ir != Ok) { // could not reset
                printf("could not reset\n");
                return -2;
            }
        } else {
            finished = true;
        }
        es++;
    }
    if (!finished) {
        // no endanalyzer was found, or the analyzer did
        // not read all of the stream, so we do that here
        StreamStatus r;
        do {
            r = input->skip(1000000);
        } while (r == 0);
        if (r == Error) {
            printf("%s\n", input->getError().c_str());
            return -2;
        }
    }

    // iterator must be reinitialized because vector may
    // have changed
    printf("%s\n", path.c_str());
    tIter = through.begin() + depth;
    for (ts = tIter->begin(); ts != tIter->end(); ++ts) {
        (*ts)->printResults();
    }
    return 0;
}

#include "diranalyzer.h"
#include "indexwriter.h"
#include "filelister.h"
#include "analysisresult.h"
#include "strigi_thread.h"
#include "fileinputstream.h"
#include <sys/stat.h>

using namespace Strigi;
using namespace std;

class DirAnalyzer::Private {
public:
    FileLister lister;
    IndexWriter& writer;
    AnalyzerConfiguration& config;
    StreamAnalyzer analyzer;

    Private(IndexWriter& w, AnalyzerConfiguration& c)
            :lister(&c), writer(w), config(c), analyzer(c) {
        analyzer.setIndexWriter(writer);
    }
    int analyzeDir(const string& dir, int nthreads);
    void analyze(StreamAnalyzer*);
};

struct DA {
    StreamAnalyzer* streamanalyzer;
    DirAnalyzer::Private* diranalyzer;
};

void*
analyzeInThread(void* d) {
    DA* a = static_cast<DA*>(d);
    a->diranalyzer->analyze(a->streamanalyzer);
    delete a;
    STRIGI_THREAD_EXIT(0);
}

DirAnalyzer::DirAnalyzer(IndexWriter& writer, AnalyzerConfiguration* conf)
        :p(new Private(writer, *conf)) {
}
DirAnalyzer::~DirAnalyzer() {
    delete p;
}
void
DirAnalyzer::Private::analyze(StreamAnalyzer* analyzer) {
    try {
        string path;
        time_t mtime;
        int r = lister.nextFile(path, mtime);
        while (r >= 0) {
            if (r > 0) {
                AnalysisResult analysisresult(path, mtime, writer, *analyzer);
                FileInputStream file(path.c_str());
                if (file.status() == Ok) {
                    analysisresult.index(&file);
                } else {
                    analysisresult.index(0);
                }
            }
            r = lister.nextFile(path, mtime);
        }
    } catch(...) {
        fprintf(stderr, "Unknown error\n");
    }
}
int
DirAnalyzer::analyzeDir(const string& dir, int nthreads) {
    return p->analyzeDir(dir, nthreads);
}
int
DirAnalyzer::Private::analyzeDir(const string& dir, int nthreads) {
    // check if the path is a file
    struct stat s;
    if (stat(dir.c_str(), &s) == -1) return -1;

    if (S_ISREG(s.st_mode)) {
        AnalysisResult analysisresult(dir, s.st_mtime, writer, analyzer);
        FileInputStream file(dir.c_str());
        if (file.status() == Ok) {
            return analysisresult.index(&file);
        } else {
            return analysisresult.index(0);
        }
    }

    lister.startListing(dir);
    if (nthreads < 1) nthreads = 1;
    vector<StreamAnalyzer*> analyzers(nthreads);
    analyzers[0] = &analyzer;
    for (int i=1; i<nthreads; ++i) {
        analyzers[i] = new StreamAnalyzer(config);
        analyzers[i]->setIndexWriter(writer);
    }
    vector<STRIGI_THREAD_TYPE> threads;
    threads.resize(nthreads-1);
    for (int i=1; i<nthreads; i++) {
        DA* da = new DA();
        da->diranalyzer = this;
        da->streamanalyzer = analyzers[i];
        STRIGI_THREAD_CREATE(&threads[i-1], analyzeInThread, da);
    }
    analyze(analyzers[0]); 
    for (int i=1; i<nthreads; i++) {
        STRIGI_THREAD_JOIN(threads[i-1]);
        delete analyzers[i];
    }
    return 0;
}

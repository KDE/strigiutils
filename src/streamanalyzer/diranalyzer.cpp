#include "diranalyzer.h"
#include "indexwriter.h"
#include "indexmanager.h"
#include "indexreader.h"
#include "filelister.h"
#include "analysisresult.h"
#include "strigi_thread.h"
#include "fileinputstream.h"
#include <map>
#include <sys/stat.h>

using namespace Strigi;
using namespace std;

class DirAnalyzer::Private {
public:
    FileLister lister;
    IndexManager& manager;
    AnalyzerConfiguration& config;
    StreamAnalyzer analyzer;
    map<string, time_t> dbfiles;
    STRIGI_MUTEX_DEFINE(updateMutex)
    bool (*continueAnalysis)();

    Private(IndexManager& m, AnalyzerConfiguration& c)
            :lister(&c), manager(m), config(c), analyzer(c) {
        STRIGI_MUTEX_INIT(&updateMutex);
        analyzer.setIndexWriter(*manager.indexWriter());
    }
    ~Private() {
        STRIGI_MUTEX_DESTROY(&updateMutex);
    }
    int analyzeDir(const string& dir, int nthreads, bool (*continueAnalysis)());
    int updateDirs(const vector<string>& dir, int nthreads,
        bool (*continueAnalysis)());
    void analyze(StreamAnalyzer*);
    void update(StreamAnalyzer*);
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
void*
updateInThread(void* d) {
    DA* a = static_cast<DA*>(d);
    a->diranalyzer->update(a->streamanalyzer);
    delete a;
    STRIGI_THREAD_EXIT(0);
}

DirAnalyzer::DirAnalyzer(IndexManager& manager, AnalyzerConfiguration& conf)
        :p(new Private(manager, conf)) {
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
        while (r >= 0 && (continueAnalysis == 0 || continueAnalysis())) {
            if (r > 0) {
                AnalysisResult analysisresult(path, mtime,
                    *manager.indexWriter(), *analyzer);
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
void
DirAnalyzer::Private::update(StreamAnalyzer* analyzer) {
    try {
        string path;
        time_t mtime;
        int r = lister.nextFile(path, mtime);
        while (r >= 0 && (continueAnalysis == 0 || continueAnalysis())) {
            if (r > 0) {
                STRIGI_MUTEX_LOCK(&updateMutex);
                map<string, time_t>::iterator i
                    = dbfiles.find(path);
                bool newfile = i == dbfiles.end();
                bool updatedfile = !newfile && i->second != mtime;

                // if the file is not new or updated, remove it from the list
                // of files to be removed from the index
                if (!newfile && !updatedfile) {
                    dbfiles.erase(i);
                }
                STRIGI_MUTEX_UNLOCK(&updateMutex);

                // if the file has not yet been indexed or if the mtime has
                // changed, index it
                if (newfile || updatedfile) {
                    AnalysisResult analysisresult(path, mtime,
                        *manager.indexWriter(), *analyzer);
                    FileInputStream file(path.c_str());
                    if (file.status() == Ok) {
                        analysisresult.index(&file);
                    } else {
                        analysisresult.index(0);
                    }
                }
            }
            r = lister.nextFile(path, mtime);
        }
    } catch(...) {
        fprintf(stderr, "Unknown error\n");
    }
}
int
DirAnalyzer::analyzeDir(const string& dir, int nthreads, bool (*cont)()) {
    return p->analyzeDir(dir, nthreads, cont);
}
int
DirAnalyzer::Private::analyzeDir(const string& dir, int nthreads,
        bool (*cont)()) {
    continueAnalysis = cont;
    // check if the path is a file
    struct stat s;
    if (stat(dir.c_str(), &s) == -1) return -1;

    if (S_ISREG(s.st_mode)) {
        AnalysisResult analysisresult(dir, s.st_mtime,
            *manager.indexWriter(), analyzer);
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
        analyzers[i]->setIndexWriter(*manager.indexWriter());
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
int
DirAnalyzer::updateDir(const string& dir, int nthreads, bool (*cont)()) {
    vector<string> dirs;
    dirs.push_back(dir);
    return p->updateDirs(dirs, nthreads, cont);
}
int
DirAnalyzer::Private::updateDirs(const vector<string>& dirs, int nthreads,
        bool (*cont)()) {
    IndexReader* reader = manager.indexReader();
    if (reader == 0) return -1;
    continueAnalysis = cont;

    // retrieve the complete list of files
    dbfiles = reader->files(0);
    fprintf(stderr, "currently %i files\n", dbfiles.size());

    if (nthreads < 1) nthreads = 1;
    vector<StreamAnalyzer*> analyzers(nthreads);
    analyzers[0] = &analyzer;
    for (int i=1; i<nthreads; ++i) {
        analyzers[i] = new StreamAnalyzer(config);
        analyzers[i]->setIndexWriter(*manager.indexWriter());
    }
    vector<STRIGI_THREAD_TYPE> threads;
    threads.resize(nthreads-1);

    for (vector<string>::const_iterator d =dirs.begin(); d != dirs.end(); ++d) {
        lister.startListing(*d);
        for (int i=1; i<nthreads; i++) {
            DA* da = new DA();
            da->diranalyzer = this;
            da->streamanalyzer = analyzers[i];
            STRIGI_THREAD_CREATE(&threads[i-1], updateInThread, da);
        }
        update(analyzers[0]); 
        for (int i=1; i<nthreads; i++) {
            STRIGI_THREAD_JOIN(threads[i-1]);
        }
    }
    for (int i=1; i<nthreads; i++) {
        delete analyzers[i];
    }

    // remove the files that were not encountered from the index
    vector<string> todelete(dbfiles.size());
    fprintf(stderr, "to delete: %i\n", dbfiles.size());
    map<string,time_t>::iterator it = dbfiles.begin();
    while (it != dbfiles.end()) {
        todelete.push_back(it->first);
        ++it;
    }
    manager.indexWriter()->deleteEntries(todelete);
    dbfiles.clear();

    return 0;
}
int
DirAnalyzer::updateDirs(const std::vector<std::string>& dirs, int nthreads,
        bool (*continueAnalysis)()) {
    return p->updateDirs(dirs, nthreads, continueAnalysis);
}

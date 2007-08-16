/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "diranalyzer.h"
#include "indexwriter.h"
#include "indexmanager.h"
#include "indexreader.h"
#include "filelister.h"
#include "analysisresult.h"
#include "strigi_thread.h"
#include "fileinputstream.h"
#include <map>
#include <iostream>
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
    STRIGI_MUTEX_DEFINE(updateMutex);
    AnalysisCaller* caller;

    Private(IndexManager& m, AnalyzerConfiguration& c)
            :lister(&c), manager(m), config(c), analyzer(c) {
        STRIGI_MUTEX_INIT(&updateMutex);
        analyzer.setIndexWriter(*manager.indexWriter());
    }
    ~Private() {
        STRIGI_MUTEX_DESTROY(&updateMutex);
    }
    int analyzeDir(const string& dir, int nthreads, AnalysisCaller* caller,
        const string& lastToSkip);
    int updateDirs(const vector<string>& dir, int nthreads,
        AnalysisCaller* caller);
    void analyze(StreamAnalyzer*);
    void update(StreamAnalyzer*);
};

struct DA {
    StreamAnalyzer* streamanalyzer;
    DirAnalyzer::Private* diranalyzer;
};

extern "C" // Linkage for functions passed to pthread_create matters
{
void*
analyzeInThread(void* d) {
    DA* a = static_cast<DA*>(d);
    a->diranalyzer->analyze(a->streamanalyzer);
    delete a;
    STRIGI_THREAD_EXIT(0);
    return 0; // Return bogus value
}
void*
updateInThread(void* d) {
    DA* a = static_cast<DA*>(d);
    a->diranalyzer->update(a->streamanalyzer);
    delete a;
    STRIGI_THREAD_EXIT(0);
    return 0; // Return bogus value
}
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
        while (r >= 0 && (caller == 0 || caller->continueAnalysis())) {
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
    vector<string> toDelete(1);
    try {
        string path;
        time_t mtime;
        // loop over all files that exist in the index
        int r = lister.nextFile(path, mtime);
        while (r >= 0 && (caller == 0 || caller->continueAnalysis())) {
            if (r > 0) {
                // check if this file is new or not
                STRIGI_MUTEX_LOCK(&updateMutex);
                map<string, time_t>::iterator i = dbfiles.find(path);
                bool newfile = i == dbfiles.end();
                bool updatedfile = !newfile && i->second != mtime;

                // if the file is update we delete it in this loop
                // if it is new, it should not be in the list anyway
                // otherwise we should _not_ delete it and remove it from the
                // to be deleted list
                if (updatedfile || !newfile) {
                    dbfiles.erase(i);
                }
                STRIGI_MUTEX_UNLOCK(&updateMutex);

                // if the file has not yet been indexed or if the mtime has
                // changed, index it
                if (updatedfile) {
                    toDelete[0].assign(path);
                    manager.indexWriter()->deleteEntries(toDelete);
                }
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
DirAnalyzer::analyzeDir(const string& dir, int nthreads, AnalysisCaller* c,
        const string& lastToSkip) {
    return p->analyzeDir(dir, nthreads, c, lastToSkip);
}
int
DirAnalyzer::Private::analyzeDir(const string& dir, int nthreads,
        AnalysisCaller* c, const string& lastToSkip) {
    caller = c;
    // check if the path exists and if it is a file or a directory
    struct stat s;
    int retval = stat(dir.c_str(), &s);
    time_t mtime = (retval == -1) ?0 :s.st_mtime;
    if (retval == -1 || S_ISREG(s.st_mode)) {
        { // at the end of this block the analysisresult is deleted and indexed
            AnalysisResult analysisresult(dir, mtime, *manager.indexWriter(),
                analyzer);
            FileInputStream file(dir.c_str());
            if (file.status() == Ok) {
                retval = analysisresult.index(&file);
            } else {
                retval = analysisresult.index(0);
            }
        }
        manager.indexWriter()->commit();
        return retval;
    }

    lister.startListing(dir);
    if (lastToSkip.length()) {
        lister.skipTillAfter(lastToSkip);
    }

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
    manager.indexWriter()->commit();
    return 0;
}
int
DirAnalyzer::updateDir(const string& dir, int nthreads, AnalysisCaller* caller){
    vector<string> dirs;
    dirs.push_back(dir);
    return p->updateDirs(dirs, nthreads, caller);
}
int
DirAnalyzer::Private::updateDirs(const vector<string>& dirs, int nthreads,
        AnalysisCaller* c) {
    IndexReader* reader = manager.indexReader();
    if (reader == 0) return -1;
    caller = c;

    // retrieve the complete list of files
    dbfiles = reader->files(0);

    // create the streamanalyzers
    if (nthreads < 1) nthreads = 1;
    vector<StreamAnalyzer*> analyzers(nthreads);
    analyzers[0] = &analyzer;
    for (int i=1; i<nthreads; ++i) {
        analyzers[i] = new StreamAnalyzer(config);
        analyzers[i]->setIndexWriter(*manager.indexWriter());
    }
    vector<STRIGI_THREAD_TYPE> threads;
    threads.resize(nthreads-1);

    // loop over all directories that should be updated
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
    // clean up the analyzers
    for (int i=1; i<nthreads; i++) {
        delete analyzers[i];
    }

    // remove the files that were not encountered from the index
    vector<string> todelete(1);
    map<string,time_t>::iterator it = dbfiles.begin();
    while (it != dbfiles.end()) {
        todelete[0].assign(it->first);
        manager.indexWriter()->deleteEntries(todelete);
        ++it;
    }
    dbfiles.clear();

    return 0;
}
int
DirAnalyzer::updateDirs(const std::vector<std::string>& dirs, int nthreads,
        AnalysisCaller* caller) {
    return p->updateDirs(dirs, nthreads, caller);
}

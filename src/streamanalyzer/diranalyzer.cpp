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
    DirLister dirlister;
    IndexManager& manager;
    AnalyzerConfiguration& config;
    StreamAnalyzer analyzer;
    STRIGI_MUTEX_DEFINE(updateMutex);
    AnalysisCaller* caller;

    Private(IndexManager& m, AnalyzerConfiguration& c)
            :dirlister(&c), manager(m), config(c), analyzer(c) {
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
    int analyzeFile(const string& path, time_t mtime, bool realfile);
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
int
DirAnalyzer::Private::analyzeFile(const string& path, time_t mtime,
        bool realfile) {
    AnalysisResult analysisresult(path, mtime, *manager.indexWriter(),
        analyzer, "");
    if (realfile) {
        FileInputStream file(path.c_str());
        return analysisresult.index(&file);
    } else {
        return analysisresult.index(0);
    }
}
void
DirAnalyzer::Private::analyze(StreamAnalyzer* analyzer) {
    IndexWriter& indexWriter = *manager.indexWriter();
    try {
        string parentpath;
        vector<pair<string, struct stat> > dirfiles;
        int r = dirlister.nextDir(parentpath, dirfiles);

        while (r == 0 && (caller == 0 || caller->continueAnalysis())) {
            vector<pair<string, struct stat> >::const_iterator end
                = dirfiles.end();
            for (vector<pair<string, struct stat> >::const_iterator i
                    = dirfiles.begin(); i != end; ++i) {
                const string& filepath(i->first);
                struct stat s = i->second;
                AnalysisResult analysisresult(filepath, s.st_mtime,
                    indexWriter, *analyzer, parentpath);
                if (S_ISREG(s.st_mode)) {
                    FileInputStream file(filepath.c_str());
                    analysisresult.index(&file);
                } else {
                    analysisresult.index(0);
                }
            }
            r = dirlister.nextDir(parentpath, dirfiles);
        }
    } catch(...) {
        fprintf(stderr, "Unknown error\n");
    }
}
void
DirAnalyzer::Private::update(StreamAnalyzer* analyzer) {
    IndexReader* reader = manager.indexReader();
    vector<pair<string, struct stat> > dirfiles;
    map<string, time_t> dbdirfiles;
    vector<string> toDelete;
    vector<pair<string, struct stat> > toIndex;
    try {
        string path;
        // loop over all files that exist in the index
        int r = dirlister.nextDir(path, dirfiles);
        while (r >= 0 && (caller == 0 || caller->continueAnalysis())) {
            if (r < 0) {
                continue;
            }
            // get the files that are in the current database
            reader->getChildren(path, dbdirfiles);

            // get all files in this directory
            vector<pair<string, struct stat> >::const_iterator end
                = dirfiles.end();
            map<string, time_t>::const_iterator dbend = dbdirfiles.end();
            for (vector<pair<string, struct stat> >::const_iterator i
                    = dirfiles.begin(); i != end; ++i) {
                const string& filepath(i->first);
                time_t mtime = i->second.st_mtime;

                // check if this file is new or not
                map<string, time_t>::iterator j = dbdirfiles.find(filepath);
                bool newfile = j == dbend;
                bool updatedfile = !newfile && j->second != mtime;

                // if the file is update we delete it in this loop
                // if it is new, it should not be in the list anyway
                // otherwise we should _not_ delete it and remove it from the
                // to be deleted list
                if (updatedfile || !newfile) {
                    dbdirfiles.erase(j);
                }
                // if the file has not yet been indexed or if the mtime has
                // changed, index it
                if (updatedfile) {
                    toDelete.push_back(filepath);
                }
                if (newfile || updatedfile) {
                    toIndex.push_back(make_pair(filepath, i->second));
                }
            }
            // all the files left in dbdirfiles, are not in the current
            // directory and should be deleted
            for (map<string, time_t>::const_iterator i = dbdirfiles.begin();
                    i != dbend; ++i) {
                toDelete.push_back(i->first);
            }
            manager.indexWriter()->deleteEntries(toDelete);
            vector<pair<string, struct stat> >::const_iterator fend
                = toIndex.end();
            for (vector<pair<string, struct stat> >::const_iterator i
                    = toIndex.begin(); i != fend; ++i) {
                AnalysisResult analysisresult(i->first, i->second.st_mtime,
                    *manager.indexWriter(), *analyzer, path);
                if (S_ISREG(i->second.st_mode)) {
                    FileInputStream file(i->first.c_str());
                    analysisresult.index(&file);
                } else {
                    analysisresult.index(0);
                }
            }
            toDelete.clear();
            toIndex.clear();
            r = dirlister.nextDir(path, dirfiles);
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
namespace {
string
removeTrailingSlash(const string& path) {
    string cleanpath(path);
    if (path.length() && path[path.length()-1] == '/') {
        cleanpath.resize(path.length()-1);
    }
    return cleanpath;
}
}
int
DirAnalyzer::Private::analyzeDir(const string& dir, int nthreads,
        AnalysisCaller* c, const string& lastToSkip) {
    caller = c;
    // check if the path exists and if it is a file or a directory
    struct stat s;
    int retval = stat(dir.c_str(), &s);
    time_t mtime = (retval == -1) ?0 :s.st_mtime;
    retval = analyzeFile(dir, mtime, S_ISREG(s.st_mode));
    // if the path does not point to a directory, return
    if (!S_ISDIR(s.st_mode)) {
        manager.indexWriter()->commit();
        return retval;
    }

    dirlister.startListing(removeTrailingSlash(dir));
    if (lastToSkip.length()) {
        dirlister.skipTillAfter(lastToSkip);
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
        dirlister.startListing(removeTrailingSlash(*d));
        for (int i=1; i<nthreads; i++) {
            DA* da = new DA();
            da->diranalyzer = this;
            da->streamanalyzer = analyzers[i];
            STRIGI_THREAD_CREATE(&threads[i-1], updateInThread, da);
        }
        update(analyzers[0]);
        // wait until all threads have finished
        for (int i=1; i<nthreads; i++) {
            STRIGI_THREAD_JOIN(threads[i-1]);
        }
        dirlister.stopListing();
    }
    // clean up the analyzers
    for (int i=1; i<nthreads; i++) {
        delete analyzers[i];
    }

    // remove the files that were not encountered from the index
/*    vector<string> todelete(1);
    map<string,time_t>::iterator it = dbfiles.begin();
    while (it != dbfiles.end()) {
        todelete[0].assign(it->first);
        manager.indexWriter()->deleteEntries(todelete);
        ++it;
    }
    dbfiles.clear();*/

    return 0;
}
int
DirAnalyzer::updateDirs(const vector<string>& dirs, int nthreads,
        AnalysisCaller* caller) {
    return p->updateDirs(dirs, nthreads, caller);
}

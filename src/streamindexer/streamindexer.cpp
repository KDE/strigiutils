/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#include "jstreamsconfig.h"
#include "streamindexer.h"
#include "fileinputstream.h"
#include "streamendanalyzer.h"
#include "streamthroughanalyzer.h"
#include "bz2endanalyzer.h"
#include "textendanalyzer.h"
#include "saxendanalyzer.h"
#include "tarendanalyzer.h"
#include "arendanalyzer.h"
#include "zipendanalyzer.h"
#include "rpmendanalyzer.h"
#include "pngendanalyzer.h"
#include "gzipendanalyzer.h"
#include "mailendanalyzer.h"
#include "helperendanalyzer.h"
#include "id3v2throughanalyzer.h"
#include "digestthroughanalyzer.h"
#include "pluginthroughanalyzer.h"
#include "pluginendanalyzer.h"
#include "indexable.h"
#include "textutils.h"
#include <sys/stat.h>

using namespace std;
using namespace jstreams;

StreamIndexer::StreamIndexer(IndexWriter* w) :writer(w) {
   
    moduleLoader.loadPlugins("/usr/local/lib/strigi");
    moduleLoader.loadPlugins("/usr/lib/strigi");
    moduleLoader.loadPlugins("/lib/strigi");

    // todo: remove this
    moduleLoader.loadPlugins("D:\\clients\\strigi_svn\\win\\out\\Debug");
    if ( getenv("HOME") != NULL ){
        string homedir = getenv("HOME");
        homedir += "/testinstall/lib/strigi";
        moduleLoader.loadPlugins(homedir.c_str());
    }
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
    return indexFile(path);
}
char
StreamIndexer::indexFile(const std::string& filepath) {
    if (!checkUtf8(filepath.c_str())) {
        return 1;
    }
    struct stat s;
    stat(filepath.c_str(), &s);
    FileInputStream file(filepath.c_str());
    // ensure a decent buffer size
    //file.mark(65530);
    string name;
    Indexable indexable(filepath, s.st_mtime, *writer, *this);
    return indexable.index(file);
}
void
StreamIndexer::addThroughAnalyzers() {
    through.resize(through.size()+1);
    std::vector<std::vector<StreamThroughAnalyzer*> >::reverse_iterator tIter;
    tIter = through.rbegin();
    StreamThroughAnalyzer* ana = new DigestThroughAnalyzer();
    tIter->push_back(ana);
    ana = new PluginThroughAnalyzer(&moduleLoader);
    tIter->push_back(ana);
    ana = new ID3V2ThroughAnalyzer();
    tIter->push_back(ana);
}
void
StreamIndexer::addEndAnalyzers() {
    end.resize(end.size()+1);
    std::vector<std::vector<StreamEndAnalyzer*> >::reverse_iterator eIter;
    eIter = end.rbegin();
    StreamEndAnalyzer* ana = new BZ2EndAnalyzer();
    eIter->push_back(ana);
    ana = new GZipEndAnalyzer();
    eIter->push_back(ana);
    ana = new TarEndAnalyzer();
    eIter->push_back(ana);
    ana = new ArEndAnalyzer();
    eIter->push_back(ana);
    ana = new MailEndAnalyzer();
    eIter->push_back(ana);
    ana = new ZipEndAnalyzer();
    eIter->push_back(ana);
    ana = new RpmEndAnalyzer();
    eIter->push_back(ana);
    ana = new PngEndAnalyzer();
    eIter->push_back(ana);
    ana = new PluginEndAnalyzer(&moduleLoader);
    eIter->push_back(ana);

/*    ana = new PdfEndAnalyzer();
    eIter->push_back(ana);*/
    // add a sax analyzer before the text analyzer
    ana = new SaxEndAnalyzer();
    eIter->push_back(ana);
    ana = new HelperEndAnalyzer();
    eIter->push_back(ana);
    // add a text analyzer to the end of the queue
    ana = new TextEndAnalyzer();
    eIter->push_back(ana);
}
char
StreamIndexer::analyze(Indexable& idx, jstreams::StreamBase<char>* input) {
//    static int count = 1;
//    if (++count % 1000 == 0) {
//        fprintf(stderr, "file #%i: %s\n", count, path.c_str());
//    }
    //printf("depth #%i: %s\n", depth, path.c_str());
   
    // retrieve or construct the through analyzers and end analyzers
    std::vector<std::vector<StreamThroughAnalyzer*> >::iterator tIter;
    std::vector<std::vector<StreamEndAnalyzer*> >::iterator eIter;
    while ((int)through.size() <= idx.getDepth()) {
        addThroughAnalyzers();
        addEndAnalyzers();
    }
    tIter = through.begin() + idx.getDepth();
    eIter = end.begin() + idx.getDepth();

    // insert the through analyzers
    std::vector<StreamThroughAnalyzer*>::iterator ts;
    for (ts = tIter->begin(); ts != tIter->end(); ++ts) {
        (*ts)->setIndexable(&idx);
        input = (*ts)->connectInputStream(input);
    }
    bool finished = false;
    int32_t headersize = 1024;
    const char* header;
    headersize = input->read(header, headersize, 0);
    if (input->reset(0) != 0) {
        fprintf(stderr, "resetting is impossible!! pos: %lli status: %i\n",
            input->getPosition(), input->getStatus());
    }
    if (headersize < 0) finished = true;
    int es = 0, size = eIter->size();
    while (!finished && es != size) {
        StreamEndAnalyzer* sea = (*eIter)[es];
        if (sea->checkHeader(header, headersize)) {
            char ar = sea->analyze(idx, input);
            if (ar) {
                int64_t pos = input->reset(0);
                if (pos != 0) { // could not reset
                    fprintf(stderr, "could not reset stream of %s from pos "
                        "%lli to 0 after reading with %s: %s\n",
                        idx.getPath().c_str(), input->getPosition(),
                        sea->getName(), sea->getError().c_str());
                    removeIndexable(idx.getDepth());
                    return -2;
                }
            } else {
                finished = true;
            }
            eIter = end.begin() + idx.getDepth();
        }
        es++;
    }
    // make sure the entire stream as read
    bool ready;
    tIter = through.begin() + idx.getDepth();
    do {
        ready = true;
        std::vector<StreamThroughAnalyzer*>::iterator ts;
        for (ts = tIter->begin(); ready && ts != tIter->end(); ++ts) {
            ready = (*ts)->isReadyWithStream();
        }
        if (!ready) {
            input->skip(1000000);
        }
    } while (!ready && input->getStatus() == Ok);
    if (input->getStatus() == Error) {
        fprintf(stderr, "Error: %s\n", input->getError());
        removeIndexable(idx.getDepth());
        return -2;
    }

    // store the size of the stream
    {  
        //tmp scope out tmp mem
        char tmp[100];
        sprintf(tmp, "%lli", input->getSize());
        idx.setField("size", tmp);
    }

    // remove references to the indexable before it goes out of scope
    removeIndexable(idx.getDepth());
    return 0;
}
void
StreamIndexer::removeIndexable(uint depth) {
    std::vector<std::vector<StreamThroughAnalyzer*> >::iterator tIter;
    std::vector<StreamThroughAnalyzer*>::iterator ts;
    tIter = through.begin() + depth;
    for (ts = tIter->begin(); ts != tIter->end(); ++ts) {
        // remove references to the indexable before it goes out of scope
        (*ts)->setIndexable(0);
    }
}

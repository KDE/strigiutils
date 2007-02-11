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
#include "bmpendanalyzer.h"
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
#include "oggthroughanalyzer.h"
#include "digestthroughanalyzer.h"
#include "indexable.h"
#include "indexerconfiguration.h"
#include "textutils.h"
#include "analyzerloader.h"
#include <sys/stat.h>
#ifdef WIN32
 #include "ifilterendanalyzer.h"
#endif
#include <config.h>

using namespace std;
using namespace jstreams;

cnstr StreamIndexer::sizefieldname("size");

StreamIndexer::StreamIndexer(IndexerConfiguration& c)
        :conf(c) {
    moduleLoader = new AnalyzerLoader();
    sizefield = c.getFieldRegister().registerField(sizefieldname,
        FieldRegister::integerType, 1, 0);

    moduleLoader->loadPlugins( LIBINSTALLDIR "/strigi");
    
        // todo: remove this
    moduleLoader->loadPlugins("D:\\clients\\strigi_svn\\win\\out\\Debug");
        if ( getenv("HOME") != NULL ){
            string homedir = getenv("HOME");
            homedir += "/testinstall/lib/strigi";
        moduleLoader->loadPlugins(homedir.c_str());
        }
    initializeThroughFactories();
    initializeEndFactories();
}
StreamIndexer::~StreamIndexer() {
    // delete all factories
    std::vector<StreamThroughAnalyzerFactory*>::iterator ta;
    for (ta = throughfactories.begin(); ta != throughfactories.end(); ++ta) {
        delete *ta;
    }
    std::vector<StreamEndAnalyzerFactory*>::iterator ea;
    for (ea = endfactories.begin(); ea != endfactories.end(); ++ea) {
        delete *ea;
    }
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
    delete moduleLoader;
}
char
StreamIndexer::indexFile(const char *filepath, IndexWriter& writer) {
    std::string path(filepath);
    return indexFile(path, writer);
}
char
StreamIndexer::indexFile(const std::string& filepath, IndexWriter& writer) {
    if (!checkUtf8(filepath.c_str())) {
        return 1;
    }
    struct stat s;
    stat(filepath.c_str(), &s);
    FileInputStream file(filepath.c_str());
    // ensure a decent buffer size
    //file.mark(65530);
    string name;
    Indexable indexable(filepath, s.st_mtime, writer, *this);
    return indexable.index(file);
}
void
StreamIndexer::addFactory(StreamThroughAnalyzerFactory* f) {
    if (conf.useFactory(f)) {
        throughfactories.push_back(f);
    } else {
        delete f;
    }
}
void
StreamIndexer::initializeThroughFactories() {
    list<StreamThroughAnalyzerFactory*> plugins
        = moduleLoader->getStreamThroughAnalyzerFactories();
    list<StreamThroughAnalyzerFactory*>::iterator i;
    for (i = plugins.begin(); i != plugins.end(); ++i) {
        addFactory(*i);
    }
    //addFactory(new DigestThroughAnalyzerFactory());
    addFactory(new ID3V2ThroughAnalyzerFactory());
    addFactory(new OggThroughAnalyzerFactory());
}
void
StreamIndexer::addFactory(StreamEndAnalyzerFactory* f) {
    f->registerFields(conf.getFieldRegister());
    if (conf.useFactory(f)) {
        endfactories.push_back(f);
    } else {
        delete f;
    }
}
/**
 * Instantiate factories for all analyzers.
 **/
void
StreamIndexer::initializeEndFactories() {
    list<StreamEndAnalyzerFactory*> plugins
        = moduleLoader->getStreamEndAnalyzerFactories();
    list<StreamEndAnalyzerFactory*>::iterator i;
    for (i = plugins.begin(); i != plugins.end(); ++i) {
        addFactory(*i);
    }
    addFactory(new BZ2EndAnalyzerFactory());
    addFactory(new GZipEndAnalyzerFactory());
    addFactory(new TarEndAnalyzerFactory());
    addFactory(new ArEndAnalyzerFactory());
    addFactory(new MailEndAnalyzerFactory());
    addFactory(new ZipEndAnalyzerFactory());
    addFactory(new RpmEndAnalyzerFactory());
    addFactory(new PngEndAnalyzerFactory());
    addFactory(new BmpEndAnalyzerFactory());
//    addFactory(new PdfEndAnalyzerFactory());
#ifdef WIN32
    addFactory(new IFilterEndAnalyzerFactory());
#else
	//temporary only, i just haven't got expat.h working yet
	addFactory(new SaxEndAnalyzerFactory());
#endif
    addFactory(new HelperEndAnalyzerFactory());
    addFactory(new TextEndAnalyzerFactory());
}
void
StreamIndexer::addThroughAnalyzers() {
    through.resize(through.size()+1);
    std::vector<std::vector<StreamThroughAnalyzer*> >::reverse_iterator tIter;
    tIter = through.rbegin();
    std::vector<StreamThroughAnalyzerFactory*>::iterator ta;
    for (ta = throughfactories.begin(); ta != throughfactories.end(); ++ta) {
        tIter->push_back((*ta)->newInstance());
    }
}
void
StreamIndexer::addEndAnalyzers() {
    end.resize(end.size()+1);
    std::vector<std::vector<StreamEndAnalyzer*> >::reverse_iterator eIter;
    eIter = end.rbegin();
    std::vector<StreamEndAnalyzerFactory*>::iterator ea;
    for (ea = endfactories.begin(); ea != endfactories.end(); ++ea) {
        eIter->push_back((*ea)->newInstance());
    }
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
    headersize = input->read(header, headersize, headersize);
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
                    finished = true;
                }
            } else {
                finished = true;
            }
            eIter = end.begin() + idx.getDepth();
        }
        es++;
    }
    // make sure the entire stream is read
    bool ready;
    tIter = through.begin() + idx.getDepth();
    do {
        ready = input->getSize() != -1;
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
        idx.setField(sizefield, tmp);
    }

    // remove references to the indexable before it goes out of scope
    removeIndexable(idx.getDepth());
    return 0;
}
/**
 * Remove references to the indexable before it goes out of scope.
 **/
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

/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2009 Jos van den Oever <jos@vandenoever.info>
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
/*
 * Some relatively high level tests for checking the indexer.
 * Writing of tests is often triggered by bugs encountered, which makes the collection look miscellaneous.
 */

#include "analyzerconfiguration.h"
#include "indexwriter.h"
#include "indexmanager.h"
#include "analysisresult.h"
#include "diranalyzer.h"
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <cerrno>
#include <cstring>
#ifndef WIN32
	#include <unistd.h>
#else
	#include <direct.h>
#endif

/**
 * Indexer configuration that lists only the paths of real files.
 **/
class FindIndexerConfiguration : public Strigi::AnalyzerConfiguration {
public:
    bool useFactory(Strigi::StreamEndAnalyzerFactory* e) const { return false; }
    bool useFactory(Strigi::StreamThroughAnalyzerFactory*) const {return false;}
    bool indexMore() const {return true;}
    bool addMoreText() const {return false;}
    FieldType indexType(const std::string& fieldname) const { return None; }
};
class FileListIndexWriter : public Strigi::IndexWriter {
private:
    std::vector<std::string> m_list;
protected:
    void startAnalysis(const Strigi::AnalysisResult* ar) {
        m_list.push_back(ar->path());
    }
    void finishAnalysis(const Strigi::AnalysisResult* ar) {}
    void addText(const Strigi::AnalysisResult* ar, const char* text,
        int32_t length) {}
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* field, const std::string& value) {}
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* fieldname, const unsigned char* data,
        uint32_t size) {}
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* fieldname, uint32_t value) {}
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* fieldname, int32_t value) {}
    void addValue(const Strigi::AnalysisResult* ar,
        const Strigi::RegisteredField* fieldname, double value) {}
    void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) {}
    void addValue(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* field, const std::string& name,
        const std::string& value) {}
public:
    FileListIndexWriter() {}
    ~FileListIndexWriter() {}
    void commit() {}
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
    std::vector<std::string> list() const { return m_list; }
};

class FileListIndexManager : public Strigi::IndexManager {
private:
    FileListIndexWriter writer;
public:
    FileListIndexManager() :writer() {}
    Strigi::IndexWriter* indexWriter() { return &writer; }
    Strigi::IndexReader* indexReader() { return 0; }
    std::vector<std::string> list() const { return writer.list(); }
};

/*
 * Test if having directories that are not accessible influences the indexing.
 * Scenario is this similar to running these command-line commands:
 *   mkdir a b c
 *   echo hi > b/hi
 *   chmod a-r a c
 *   deepfind .
 * The deepfind run should find the file 'b/hi'.
 */
int
testScanWithUnreadableDir() {
    // setup
#ifdef WIN32
    if (mkdir("testScanWithUnreadableDir") != 0
            || mkdir("testScanWithUnreadableDir/a") != 0
            || mkdir("testScanWithUnreadableDir/b") != 0
            || mkdir("testScanWithUnreadableDir/c") != 0) {
#else
    if (mkdir("testScanWithUnreadableDir", 0700) != 0
            || mkdir("testScanWithUnreadableDir/a", 0000) != 0
            || mkdir("testScanWithUnreadableDir/b", 0700) != 0
            || mkdir("testScanWithUnreadableDir/c", 0000) != 0) {
#endif
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    FILE * file = fopen("testScanWithUnreadableDir/b/hello", "w");
    if (file == NULL || fwrite("world", 1, 5, file) != 5 || fclose(file) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }

    // test
    int result = 0;
    FileListIndexManager manager;
    FindIndexerConfiguration conf;
    Strigi::DirAnalyzer analyzer(manager, conf);
    analyzer.analyzeDir("testScanWithUnreadableDir", 1);
    std::vector<std::string> list = manager.list();
    if (std::find(list.begin(), list.end(),
            "testScanWithUnreadableDir/b/hello") == list.end()) {
        fprintf(stderr, "The file 'hello' was not listed.'\n");
        result = -1;
    }

    // teardown
    if (unlink("testScanWithUnreadableDir/b/hello") != 0
            || rmdir("testScanWithUnreadableDir/a") != 0
            || rmdir("testScanWithUnreadableDir/b") != 0
            || rmdir("testScanWithUnreadableDir/c") != 0
            || rmdir("testScanWithUnreadableDir") != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    return result;
}

/*
 * Main function calling all the individual tests.
 */
int
indextests(int argc, char* argv[]) {
    if (testScanWithUnreadableDir() != 0) return -1;
    return 0;
}

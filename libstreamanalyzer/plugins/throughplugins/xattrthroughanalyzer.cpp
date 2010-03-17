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
#define STRIGI_IMPORT_API //todo: could also define this in cmake...
#include <strigi/strigiconfig.h>
#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>
#include <strigi/analysisresult.h>
#include <sys/types.h>
#include <attr/xattr.h>
#include <errno.h>
using namespace Strigi;
using namespace std;

class XattrAnalyzer : public Strigi::StreamThroughAnalyzer {
private:
    static const int maxnamesize = 262144;
    int namesize;
    char* namebuffer;
    static const int maxvalsize = 262144;
    int valsize;
    char* valbuffer;
    Strigi::AnalysisResult* idx;

    const char* retrieveAttribute(const char*);
public:
    XattrAnalyzer() {
        namebuffer = (char*)malloc(1024);
        namesize = 1024;
        valbuffer = (char*)malloc(1024);
        valsize = 1024;
    }
    ~XattrAnalyzer() {
        free(namebuffer);
        free(valbuffer);
    }
    void setIndexable(Strigi::AnalysisResult*i) {
        idx = i;
    }
    Strigi::InputStream *connectInputStream(Strigi::InputStream *in);
    bool isReadyWithStream() { return true; }
};

Strigi::InputStream *
XattrAnalyzer::connectInputStream(Strigi::InputStream *in) {
    if (idx->depth() != 0) return in;
    ssize_t s;
    errno = 0;
    do {
        if (errno == ERANGE && namesize < maxnamesize) {
            namesize *= 2;
            namebuffer = (char*)realloc(namebuffer, namesize);
        }
        s = llistxattr(idx->path().c_str(), namebuffer, namesize);
    } while (s == -1 && errno == ERANGE && namesize < maxnamesize);
    if (s == -1) return in;

    const char*start = namebuffer;
    const char*end = namebuffer;
    while (start-namebuffer < s) {
        if (*end == '\0') {
            if (end != start) {
                const char* val = retrieveAttribute(start);
                if (val) {
                    idx->addValue(start, val);
                }
                start = end+1;
            }
        }
        end++;
    }

    return in;
}

const char*
XattrAnalyzer::retrieveAttribute(const char* name) {
    ssize_t s;
    errno = 0;
    do {
        if (errno == ERANGE && valsize < maxvalsize) {
            valsize *= 2;
            valbuffer = (char*)realloc(valbuffer, valsize);
        }
        s = lgetxattr(idx->path().c_str(), name, valbuffer, valsize-1);
    } while (s == -1 && errno == ERANGE && valsize < maxvalsize);
    if (s == -1) return 0;
    valbuffer[s] = '\0';
    return valbuffer;
}

//define all the available analyzers in this plugin
class XattrThroughAnalyzerFactory : public StreamThroughAnalyzerFactory {
private:
    const char* name() const {
        return "XattrThroughAnalyzer";
    }
    StreamThroughAnalyzer* newInstance() const {
        return new XattrAnalyzer();
    }
};

class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamThroughAnalyzerFactory*>
    streamThroughAnalyzerFactories() const {
        list<StreamThroughAnalyzerFactory*> af;
        af.push_back(new XattrThroughAnalyzerFactory());
        return af;
    }
};

STRIGI_ANALYZER_FACTORY(Factory);

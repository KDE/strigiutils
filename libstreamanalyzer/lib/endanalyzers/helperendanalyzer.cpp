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

#include "helperendanalyzer.h"
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif

#include <strigi/strigiconfig.h>
//#include "compat.h"
#if !defined(_WIN32) && !defined(_WIN64)
 #include <strigi/processinputstream.h>
#endif
#include "textendanalyzer.h"
#include <strigi/analysisresult.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

using namespace Strigi;
using namespace std;

void
HelperEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    // we extract only text, no other fields
}

class HelperProgramConfig::HelperRecord {
public:
    const unsigned char* magic;
    ssize_t magicsize;
    vector<string> arguments;
    bool readfromstdin;
};

HelperProgramConfig::HelperProgramConfig() {
/*    static const unsigned char wordmagic[] = {
        0xd0,0xcf,0x11,0xe0,0xa1,0xb1,0x1a,0xe1,0,0,0,0,0,0,0,0
    };*/

    // make a vector with all the paths
    vector<string> paths;
    string path;
    if (getenv("PATH")) {
        path.assign(getenv("PATH"));
    }
    string::size_type start = 0;
    string::size_type end = path.find(':');
    while (end != string::npos) {
        if (path[start] == '/') {
            paths.push_back(path.substr(start, end-start));
        }
        start = end + 1;
        end = path.find(':', start);
    }
    if (start < path.size() && path[start] == '/') {
        paths.push_back(path.substr(start));
    }

    string exepath = findPath("pdftotext", paths);
    if (exepath.length()) {
        HelperRecord* h = new HelperRecord();
        h->magic = (unsigned char*)"%PDF-1.";
        h->magicsize = 7;
        h->arguments.push_back(exepath);
        h->arguments.push_back("-enc");
        h->arguments.push_back("UTF-8");
        h->arguments.push_back("%s");
        h->arguments.push_back("-");
        h->readfromstdin = false;
        helpers.push_back(h);
    }
    // this  does not work atm because it requires help programs itself
/*    exepath = findPath("wvWare", paths);
    if (exepath.length()) {
        HelperRecord* h = new HelperRecord();
        h->magic = wordmagic;
        h->magicsize = 16;
        h->arguments.push_back(exepath);
        h->arguments.push_back("--nographics");
        h->arguments.push_back("%s");
        h->readfromstdin = false;
        helpers.push_back(h);
    }*/
}
std::string
HelperProgramConfig::findPath(const std::string& exe,
        const std::vector<std::string>& paths) const {
    struct stat s;
    for (uint i=0; i<paths.size(); ++i) {
        string path(paths[i]);
        path += '/';
        path += exe;
        if (stat(path.c_str(), &s) == 0 && S_ISREG(s.st_mode)) {
            return path;
        }
    }
    return "";
}
HelperProgramConfig::~HelperProgramConfig() {
    vector<HelperRecord*>::const_iterator i;
    for (i = helpers.begin(); i != helpers.end(); ++i) {
        delete *i;
    }
}
HelperProgramConfig::HelperRecord*
HelperProgramConfig::findHelper(const char* header, int32_t headersize) const {
    vector<HelperRecord*>::const_iterator i;
    for (i = helpers.begin(); i != helpers.end(); ++i) {
        HelperRecord* h = *i;
        if (headersize >= h->magicsize) {
            if (memcmp(header, h->magic, h->magicsize) == 0) {
                return h;
            }
        }
    }
    return 0;
}
bool
HelperEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return helperconfig.findHelper(header, headersize) != 0;
}
signed char
HelperEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in){
    if(!in)
        return -1;

    signed char state = -1;
    const char* b;
    int32_t nread = in->read(b, 1024, 0);
    in->reset(0);
    if (nread > 0) {
        HelperProgramConfig::HelperRecord* h
            = helperconfig.findHelper(b, nread);
        if (h) {
//            fprintf(stderr, "calling %s on %s\n", h->arguments[0].c_str(),
//                idx.path().c_str());
#if !defined(_WIN32) && !defined(_WIN64)
#warning this does not work on windows because processinputstream does not compile!
            if (h->readfromstdin) {
                ProcessInputStream pis(h->arguments, in);
                TextEndAnalyzer t;
                state = t.analyze(idx, &pis);
            } else {
                string filepath;
                bool fileisondisk = checkForFile(idx);
                if (fileisondisk) {
                    filepath = idx.path();
                } else {
                    filepath = writeToTempFile(in);
                }
                vector<string> args = h->arguments;
                for (uint j=0; j<args.size(); ++j) {
                    if (args[j] == "%s") {
                        args[j] = filepath;
                    }
                }
                ProcessInputStream pis(args);
                TextEndAnalyzer t;
                state = t.analyze(idx, &pis);

                if (!fileisondisk) {
                    unlink(filepath.c_str());
                }
            }
#endif
        }
    }
    if (in->status() == Error) {
        m_error = in->error();
        state = Error;
    }
    return state;
}
string
HelperEndAnalyzer::writeToTempFile(InputStream *in) const {
    string filepath = "/tmp/strigiXXXXXX";
    char* p = (char*)filepath.c_str();
    int fd = mkstemp(p);
    if (fd == -1) {
        fprintf(stderr, "Error in making tmp name: %s\n", strerror(errno));
        return "";
    }
    const char* b;
    int32_t nread = in->read(b, 1, 0);
    while (nread > 0) {
        do {
            ssize_t n = write(fd, b, nread);
            if (n == -1) {
                close(fd);
                unlink(p);
                return "";
            }
            nread -= (int32_t)n;
        } while (nread > 0);
        nread = in->read(b, 1, 0);
    }
    close(fd);
    return filepath;
}
bool
HelperEndAnalyzer::checkForFile(const AnalysisResult& idx) const {
    if (idx.depth() > 0) return false;
    struct stat s;
    if (stat(idx.path().c_str(), &s)) return false;
    return true;
}

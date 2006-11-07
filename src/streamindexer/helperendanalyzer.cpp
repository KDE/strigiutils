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
#include "helperendanalyzer.h"
#include "streamindexer.h"
#include "processinputstream.h"
#include "textendanalyzer.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
using namespace jstreams;
using namespace std;


#ifdef WIN32
    #include <fcntl.h>
    #define _S_IREAD 256
    #define _S_IWRITE 128
    int mkstemp(char *tmpl)
    {
       mktemp(tmpl);
       return open(tmpl,O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
    }
#endif

class HelperProgramConfig::HelperRecord {
public:
    const char* magic;
    ssize_t magicsize;
    vector<string> arguments;
    bool readfromstdin;
};

HelperProgramConfig::HelperProgramConfig() {
    static const char wordmagic[] = {0xd0,0xcf,0x11,0xe0,0xa1,0xb1,0x1a,0xe1,
        0,0,0,0,0,0,0,0};

    // make a vector with all the paths
    const char* path =getenv("PATH");
    vector<string> paths;
    const char* end = strchr(path, ':');
    while (end) {
        if (path[0] == '/') {
            paths.push_back(string(path, end-path));
        }
        path = end+1;
        end = strchr(path, ':');
    }
    if (path[0] == '/') {
        paths.push_back(path);
    }

    string exepath = getPath("pdftotext", paths);
    if (exepath.length()) { 
        HelperRecord* h = new HelperRecord();
        h->magic = "%PDF-1.";
        h->magicsize = 7;
        h->arguments.push_back(exepath);
        h->arguments.push_back("%s");
        h->arguments.push_back("-");
        h->readfromstdin = false;
        helpers.push_back(h);
    }
    // this  does not work atm because it requires help programs itself
    exepath = getPath("wvWare", paths);
    if (exepath.length()) {
        HelperRecord* h = new HelperRecord();
        h->magic = wordmagic;
        h->magicsize = 16;
        h->arguments.push_back(exepath);
        h->arguments.push_back("--nographics");
        h->arguments.push_back("%s");
        h->readfromstdin = false;
        helpers.push_back(h);
    }
}
std::string
HelperProgramConfig::getPath(const std::string& exe,
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
const HelperProgramConfig HelperEndAnalyzer::helperconfig;
bool
HelperEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return helperconfig.findHelper(header, headersize) != 0;
}
char
HelperEndAnalyzer::analyze(std::string filename, jstreams::InputStream *in,
        int depth, StreamIndexer *indexer, Indexable* i) {
    char state = -1;
    const char* b;
    int32_t nread = in->read(b, 1024, 0);
    in->reset(0);
    if (nread > 0) {
        HelperProgramConfig::HelperRecord* h
            = helperconfig.findHelper(b, nread);
        if (h) {
//            fprintf(stderr, "calling %s on %s\n", h->arguments[0].c_str(),
//                filename.c_str());
            if (h->readfromstdin) {
                ProcessInputStream pis(h->arguments, in);
                TextEndAnalyzer t;
                state = t.analyze(filename, &pis, depth, indexer, i);
            } else {
                string filepath;
                bool fileisondisk = checkForFile(depth, filename);
                if (fileisondisk) {
                    filepath = filename;
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
                state = t.analyze(filename, &pis, depth, indexer, i);
                if (!fileisondisk) {
                    unlink(filepath.c_str());
                }
            }
        }
    }
    if (in->getStatus() != Eof) {
        error = in->getError();
        state = -1;
    }
    return state;
}
string
HelperEndAnalyzer::writeToTempFile(jstreams::InputStream *in) const {
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
            nread -= n;
        } while (nread > 0);
        nread = in->read(b, 1, 0);
    } 
    close(fd);
    return filepath;
}
bool
HelperEndAnalyzer::checkForFile(int depth, const std::string& filename) {
    if (depth > 0) return false;
    struct stat s;
    if (stat(filename.c_str(), &s)) return false;
    return true;
}

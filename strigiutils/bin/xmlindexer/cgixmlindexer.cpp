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

#include "xmlindexwriter.h"
#include <strigi/stringstream.h>
#include <strigi/stringterminatedsubstream.h>
#include <strigi/subinputstream.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h> // getenv
using namespace Strigi;
using namespace std;

string
readHeader(InputStream* f) {
    StringTerminatedSubStream header(f, "\r\n\r\n");
    string h;
    const char* d;
    int32_t nread = header.read(d, 1000, 0);
    while (nread > 0) {
        h.append(d, nread);
        nread = header.read(d, 1000, 0);
    }
    return h;
}

/**
 * Start parsing a file. The stream must be positioned at the start of the file
 * header.
 **/
bool
parseFile(StreamAnalyzer& sa, XmlIndexManager& manager,
        InputStream* f, const string& delim) {

    string header = readHeader(f);
    string filename;
    const char* start = header.c_str();
    start = strstr(start, "filename=");
    if (start) {
        start += 9;
        const char* end = 0;
        char c = *start;
        if (c == '\'' || c == '"') {
            start += 1;
            end = strchr(start, c);
        }
        if (end) {
            filename.assign(start, end-start);
        } else {
            filename.assign(start);
        }
    }

    // analyzer the stream
    StringTerminatedSubStream stream(f, delim);
    if (filename.size()) {
        AnalysisResult result(filename, time(0), *manager.indexWriter(), sa);
        sa.analyze(result, &stream);
    }
    // read the rest of the stream
    const char* d;
    int32_t nread = stream.read(d, 1000, 0);
    while (nread > 0) {
        nread = stream.read(d, 1000, 0);
    }

    // check if this is the last file
    nread = f->read(d, 2, 2);
    return nread == 2 && *d == '\r' && d[1] == '\n';
}

int
main() {
    const TagMapping mapping(0);
    cout << "Content-Type:text/xml;charset=UTF-8\r\n\r\n"
        "<?xml version='1.0' encoding='UTF-8'?>\n<"
        << mapping.map("metadata") << ">\n";

    int len;
    const char* lenstr = getenv("CONTENT_LENGTH");
    if (lenstr == NULL || sscanf(lenstr,"%id", &len) != 1 || len < 0) {
        cout << " <error>input too small</error>\n</"
            << mapping.map("metadata") << ">\n" << flush;
        return 0;
    }
    cerr << "len " << len << endl;
    char* e = new char[len];
    if (e == 0 || fread(e, 1, len, stdin) != (size_t)len) {
        cout << " <error>cannot allocate memory</error>\n</"
            << mapping.map("metadata") << ">\n" << flush;
        return 0;
    }

    // read from stdin
    StringInputStream stream(e, len);

    // read the first line
    const char* d = NULL;
    const int32_t maxlength = 1024;
    int32_t nread = stream.read(d, maxlength, maxlength);
    stream.reset(0);

    if (nread < 1) {
        cout << " <error>input too small</error>\n</"
            << mapping.map("metadata") << ">\n" << flush;
        return 0;
    }

    // get out the delimiter 
    const char* end = d + nread;
    const char* p = d;
    while (p < end-1 && *p != '\r') p++;
    if (*p != '\r' || p[1] != '\n') {
        cout << " <error>no delimiter line</error></"
            << mapping.map("metadata") << ">\n" << flush;
        return 0;
    }
    string delim("\r\n");
    delim.append(d, p-d);

    // skip the delimiter + '\r\n'
    stream.reset(delim.length() + 2);

    // parse all files
    XmlIndexManager manager(cout, mapping);
    AnalyzerConfiguration ac;
    StreamAnalyzer sa(ac);
    sa.setIndexWriter(*manager.indexWriter());
    while (parseFile(sa, manager, &stream, delim)) {};
    cout << "</" << mapping.map("metadata") << ">\n" << flush;

    return 0;
}

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
#include <strigi/strigiconfig.h>
#include "pdfparser.h"
#include "fileinputstream.h"
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace Strigi;


extern int32_t streamcount;

int
main(int argc, char** argv) {
#ifdef _WIN32
    mkdir("out");
#else
    mkdir("out", 0777);
#endif
    streamcount = 0;
    PdfParser parser;
    PdfParser::DefaultStreamHandler streamhandler;
    PdfParser::DefaultTextHandler texthandler;
    parser.setStreamHandler(&streamhandler);
    parser.setTextHandler(&texthandler);
    for (int i=1; i<argc; ++i) {
        // check if we can read the file
        FILE* f = fopen(argv[i], "rb");
        if (f == 0) continue;
        fclose(f);

        // parse the file
        FileInputStream file(argv[i]);
        StreamStatus r = parser.parse(&file);
        if (r != Eof) {
            printf("error in %s\n", argv[i]);
        }
    }
    return 0;
}

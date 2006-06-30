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
#include "InputStreamReaderTest.h"
#include "../inputstreamreader.h"
#include "../fileinputstream.h"
#include "inputstreamtests.h"
#include <QtTest/QtTest>
using namespace jstreams;
using namespace std;

const char* testfiles[] = {"armenia", "croattx", "huseyin", "linjilu", "russmnv", "ulysses", "banviet", "danish", "jpndoc", "maopoem", "sample6", "unilang", "calblur", "esperan", "kordoc", "neural", "tongtws"};


void
InputStreamReaderTest::testStream() {
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("home.de.html");
        InputStreamReader isr(&file);
        wcharinputstreamtests[i](&isr);
    }
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("chinese_utf8.txt");
        InputStreamReader isr(&file);
        wcharinputstreamtests[i](&isr);
    }
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("chinese_ucs2.txt");
        InputStreamReader isr(&file, "UCS2");
        wcharinputstreamtests[i](&isr);
    }
    for (int i=0; i<17; ++i) {
        // compare files with different encodings but the same content
        const wchar_t* p1, *p2;
        string filename = "unichtm/ucs2/";
        filename += testfiles[i];
        filename += ".htm";
        FileInputStream file(filename.c_str());
        InputStreamReader isr(&file, "UCS2");
        int32_t nread1 = isr.read(p1, 200000, 200000);

        filename = "unichtm/utf8/";
        filename += testfiles[i];
        filename += ".htm";
        FileInputStream file2(filename.c_str());
        InputStreamReader isr2(&file2, "UTF8");
        int32_t nread2 = isr2.read(p2, 200000, 200000);

        QVERIFY(nread1 == nread2);
        QVERIFY(memcmp(p1, p2, nread1*sizeof(wchar_t))==0);
    }
}

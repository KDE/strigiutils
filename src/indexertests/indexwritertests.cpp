/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Ben van Klinken <bvklinken@gmail.com>
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
#include "strigiconfig.h"
#include "indexwritertests.h"
#include "indexwriter.h"
#include "analysisresult.h"
#include "analyzerconfiguration.h"
using namespace Strigi;

class IndexWriterTester {
private:
    IndexWriter* writer;
    StreamAnalyzer si;
public:
    IndexWriterTester(IndexWriter& w, AnalyzerConfiguration& ic)
        :writer(&w), si(ic) {
    }
    int optimize() {
        VERIFY(writer);
        if (writer == 0) return 1;
        writer->optimize();
        return 0;
    }
    int add() {
        VERIFY(writer);
        if (writer == 0) return 1;
        std::string s("a"); // we must pass a string, not a const char*
        {
            AnalysisResult i(s, 0, *writer, si);
        }
        writer->commit();

        return 0;
    }
};

IndexWriterTests::IndexWriterTests(Strigi::IndexWriter& w,
        AnalyzerConfiguration& ic)
    :tester (new IndexWriterTester(w, ic)) {
}
IndexWriterTests::~IndexWriterTests() {
    delete tester;
}

int
IndexWriterTests::testAll() {
    int n = 0;
    n += tester->add();
    n += tester->optimize();
    return n;
}

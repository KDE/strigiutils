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
#include <stdio.h>

#include <strigi/kmpsearcher.h>
#include "../sharedtestcode/inputstreamtests.h"
#include <cstring>

using namespace Strigi;

namespace {
void
testSearch(const KmpSearcher& searcher, const char* haystack, int32_t len,
        int32_t pos) {
    if (len < 0) len = (int32_t)std::strlen(haystack);
    const char *p = searcher.search(haystack, len);
    if (pos < 0) {
        VERIFY(p==0);
    } else {
        VERIFY(p-haystack==pos);
    }
}
}

int
KmpSearcherTest(int argc, char* argv[]) {

    founderrors = 0;

    KmpSearcher searcher;
    searcher.setQuery("a");
    testSearch(searcher, "a", 0, -1);
    testSearch(searcher, "a", 1, 0);
    testSearch(searcher, "b", 1, -1);
    testSearch(searcher, "aa", 1, 0);
    testSearch(searcher, "aa", 2, 0);
    testSearch(searcher, "ba", 1, -1);
    testSearch(searcher, "ba", 2, 1);
    const char a[] = {'a'};
    testSearch(searcher, a, 1, 0);

    searcher.setQuery("aaa");
    testSearch(searcher, "a", 0, -1);
    testSearch(searcher, "a", 1, -1);
    testSearch(searcher, "b", 1, -1);
    testSearch(searcher, "aa", 1, -1);
    testSearch(searcher, "aa", 2, -1);
    testSearch(searcher, "ba", 1, -1);
    testSearch(searcher, "ba", 2, -1);
    testSearch(searcher, "baaab", -1, 1);
    testSearch(searcher, "baabaabaabaabaabaabaabaabaabaab", -1, -1);
    testSearch(searcher, "baabaabaabaabaabaabaabaabaabaaa", -1, 28);


    return founderrors;
}


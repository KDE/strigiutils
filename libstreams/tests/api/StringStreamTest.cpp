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
#include <strigi/stringstream.h>
#include "../sharedtestcode/inputstreamtests.h"
#include <cstring>

using namespace Strigi;

int
StringStreamTest(int argc, char** argv) {
    if (argc < 2) return 1;
    VERIFY(chdir(argv[1]) == 0);

    founderrors = 0;
    std::string s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i=0; i<ninputstreamtests; ++i) {
        StringStream<char> s1(s.c_str(), (int32_t)s.length());
        charinputstreamtests[i](&s1);
    }
    for (int i=0; i<ninputstreamtests; ++i) {
        StringStream<char> s1(s.c_str(), (int32_t)s.length());
        UnknownSizeInputStream s2(&s1);
        charinputstreamtests[i](&s2);
    }
    return founderrors;
}


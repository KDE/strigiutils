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
#include "../fileinputstream.h"
#include "../stringstream.h"
#include "../signatureinputstream.h"
#include "inputstreamtests.h"
#include <iostream>

using namespace std;
using namespace Strigi;

int
SignatureInputStreamTest(int argc, char* argv[]) {
    if (argc < 2) return 1;
    founderrors = 0;
    VERIFY(chdir(argv[1]) == 0);

    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.zip");
        SignatureInputStream sig(&file, 10);
        charinputstreamtests[i](&sig);
    }
    for (int i=0; i<ninputstreamtests; ++i) {
        FileInputStream file("a.zip");
        UnknownSizeInputStream ui(&file);
        SignatureInputStream sig(&ui, 10);
        charinputstreamtests[i](&sig);
    }

    std::string s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // test on stream of known size with read
    for (uint32_t i=0; i<s.length(); ++i) {
        for (int32_t j=1; j<(int32_t)s.length(); ++j) {
            StringInputStream str(s.c_str(), (int32_t)s.length(), false);
            SignatureInputStream sig(&str, i);
            const char* data;
            while (sig.read(data, j, j) > 0) {}
            VERIFY(sig.signature() == s.substr(s.length()-i));
        }
    }
    // test on stream of known size with skip
    for (uint32_t i=0; i<s.length(); ++i) {
        for (int32_t j=1; j<(int32_t)s.length(); ++j) {
            StringInputStream str(s.c_str(), (int32_t)s.length(), false);
            SignatureInputStream sig(&str, i);
            while (sig.skip(j) > 0) {}
            VERIFY(sig.signature() == s.substr(s.length()-i));
        }
    }
    // test on stream of unknown size with read
    for (uint32_t i=0; i<s.length(); ++i) {
        for (int32_t j=1; j<(int32_t)s.length(); ++j) {
            StringInputStream str(s.c_str(), (int32_t)s.length(), false);
            UnknownSizeInputStream ui(&str);
            SignatureInputStream sig(&ui, i);
            const char* data;
            while (sig.read(data, j, j) > 0) {}
            VERIFY(ui.size() == (int64_t)s.length());
            VERIFY(sig.signature() == s.substr(s.length()-i));
        }
    }
    // test on stream of unknown size with skip
    for (uint32_t i=0; i<s.length(); ++i) {
        for (int32_t j=1; j<(int32_t)s.length(); ++j) {
            StringInputStream str(s.c_str(), (int32_t)s.length(), false);
            UnknownSizeInputStream ui(&str);
            SignatureInputStream sig(&ui, i);
            while (sig.skip(j) > 0) {}
            VERIFY(ui.size() == (int64_t)s.length());
            VERIFY(sig.signature() == s.substr(s.length()-i));
        }
    }

    return founderrors;
}


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
#include <strigi/dataeventinputstream.h>
#include "../sharedtestcode/inputstreamtests.h"

using namespace Strigi;

class TestEventHandler : public DataEventHandler {
private:
    const bool done;
public:
    TestEventHandler(bool d) :done(d) {}
    bool handleData(const char* data, uint32_t size) { return done; }
};

int
EventInputStreamTest(int argc, char* argv[]) {
    if (argc < 2) return 1;
    founderrors = 0;
    VERIFY(chdir(argv[1]) == 0);

    TestEventHandler de1(true);
    TESTONFILE2(DataEventInputStream, de1, "a.zip");
    TestEventHandler de2(false);
    TESTONFILE2(DataEventInputStream, de2, "a.zip");

    return founderrors;
}


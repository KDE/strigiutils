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

#include "testinterface.h"
#include <stdio.h>
using namespace std;

std::string
TestInterface::helloWorld(const std::string& name) {
    return "Hello "+name;
}
std::string
TestInterface::concat(const std::string& a, const std::string& b) {
    return a+b;
}
bool
TestInterface::upload(const std::vector<char>& upload) {
    printf("got file of size %i uploaded\n", (int)upload.size());
    return true;
}
map<string, string>
TestInterface::giveMap() {
    map<string, string> m;
    m["a"] = "b";
    return m;
}

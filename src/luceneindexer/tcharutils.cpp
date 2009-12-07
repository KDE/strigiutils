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

#include "tcharutils.h"

#include <string.h>

using namespace std;

std::wstring
utf8toucs2(const char*p, const char*e) {
    wstring ucs2;
    if (e <= p) return ucs2;
    ucs2.reserve(3*(e-p));
    wchar_t w = 0;
    char nb = 0;
    while (p < e) {
        char c = *p;
        if (nb--) {
            w = (w<<6) + (c & 0x3f);
        } else if ((0xE0 & c) == 0xC0) {
            w = c & 0x1F;
            nb = 0;
        } else if ((0xF0 & c) == 0xE0) {
            w = c & 0x0F;
            nb = 1;
        } else if ((0xF8 & c) == 0xF0) {
            w = c & 0x07;
            nb = 2;
        } else {
            w = (w<<6) + (c&0x7F);
            ucs2 += w;
            w = 0;
            nb = 0;
        }
        p++;
    }
    return ucs2;
}
std::wstring
utf8toucs2(const char* p) {
    return utf8toucs2(p, p+strlen(p));
}
std::wstring
utf8toucs2(const std::string& utf8) {
    const char* p = utf8.c_str();
    const char* e = p + utf8.length();
    return utf8toucs2(p, e);
}
std::string
wchartoutf8(const wchar_t* p, const wchar_t* e) {
    string utf8;
    utf8.reserve((int)(1.5*(float)(e-p)));
    while (p < e) {
        wchar_t c = *p;
        if (c < 0x80) {
            utf8 += (char)c;
        } else if (c < 0x800) {
            char c2 = (char)((c & 0x3f) | 0x80);
            utf8 += (char)((c>>6) | 0xc0);
            utf8 += c2;
        } else if (c < 0x10000) {
            char c3 = (char)((c & 0x3f) | 0x80);
            char c2 = (char)(((c>>6) & 0x3f) | 0x80);
            utf8 += (char)((c>>12) | 0xe0);
            utf8 += c2;
            utf8 += c3;
        }
        p++;
    }
    return utf8;
}
std::string
wchartoutf8(const wchar_t* p) {
    return wchartoutf8(p, p+wcslen(p));
}
std::string
wchartoutf8(const std::wstring& wchar) {
    const wchar_t *p = wchar.c_str();
    const wchar_t *e = p+wchar.length();
    return wchartoutf8(p, e);
}

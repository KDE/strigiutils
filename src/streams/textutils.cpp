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
#include "jstreamsconfig.h"
#include "textutils.h"

/**
 * Return the position of the first byte that is not valid
 * utf8. Return value of 0 means that the entire string is valid.
 * If the last character is incomplete the returned value points to the start
 * of that character and nb is set to the number of characters that is missing.
 **/
bool
jstreams::checkUtf8(const char* p, int32_t length) {
    const char* end = p + length;
    // check if the text is valid UTF-8
    char nb = 0;
    while (p < end) {
        char c = *p;
        if (nb) {
            if ((0xC0 & c) != 0x80) {
                return false;
            }
            nb--;
        } else if ((0xE0 & c) == 0xC0) {
            nb = 1;
        } else if ((0xF0 & c) == 0xE0) {
            nb = 2;
        } else if ((0xF8 & c) == 0xF0) {
            nb = 3;
        } else if (c < 0x20 && !(c == 0x9 || c == 0xA || c == 0xD)) {
            return false;
        }
        p++;
    }
    // the string is only valid utf8 if it contains only complete characters
    return nb == 0;
}
bool
jstreams::checkUtf8(const std::string& p) {
    return checkUtf8(p.c_str(), p.size());
}

// http://www.w3.org/TR/REC-xml/#charsets
/**
 * Return the position of the first byte that is not valid
 * utf8. Return value of 0 means that the entire string is valid.
 * If the last character is incomplete the returned value points to the start
 * of that character and nb is set to the number of characters that is missing.
 **/
const char*
jstreams::checkUtf8(const char* p, int32_t length, char& nb) {
    const char* end = p + length;
    const char* cs = p;
    // check if the text is valid UTF-8
    nb = 0;
    while (p < end) {
        char c = *p;
        if (nb) {
            if ((0xC0 & c) != 0x80) {
                nb = 0;
                return p;
            }
            nb--;
        } else if ((0xE0 & c) == 0xC0) {
            cs = p;
            nb = 1;
        } else if ((0xF0 & c) == 0xE0) {
            cs = p;
            nb = 2;
        } else if ((0xF8 & c) == 0xF0) {
            cs = p;
            nb = 3;
        } else if (c < 0x20 && !(c == 0x9 || c == 0xA || c == 0xD)) {
            return p;
        }
        p++;
    }
    // the string is only valid utf8 if it contains only complete characters
    return (nb) ?cs :0;
}
const char*
jstreams::checkUtf8(const std::string& p, char& nb) {
    return checkUtf8(p.c_str(), p.size(), nb);
}

/**
 * convert the \r and \n in utf8 strings into spaces
 **/
void
jstreams::convertNewLines(char* p) {
    int32_t len = strlen(p);
    bool ok = checkUtf8(p, len);
    if (!ok) fprintf(stderr, "string is not valid utf8\n");

    const char* end = p+len;
    char nb = 0;
    while (p < end) {
        char c = *p;
        if (nb) {
            if ((0xC0 & c) != 0x80) {
                return;
            }
            nb--;
        } else if ((0xE0 & c) == 0xC0) {
            nb = 1;
        } else if ((0xF0 & c) == 0xE0) {
            nb = 2;
        } else if ((0xF8 & c) == 0xF0) {
            nb = 3;
        } else if (c == '\n' || c == '\r') {
            *p = ' ';
        }
        p++;
    }
}



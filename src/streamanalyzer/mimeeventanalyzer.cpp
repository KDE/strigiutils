/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *           (C) 2006 David Faure <dfaure@kde.org>
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
#include "mimeeventanalyzer.h"
#include "fieldtypes.h"
#include "textutils.h"
#include "fileinputstream.h"

// http://standards.freedesktop.org/shared-mime-info-spec/shared-mime-info-spec-0.12.html

using namespace Strigi;
using namespace std;

class MimeRule {
public:
    uint64_t offset;
    uint64_t range;
    unsigned char* value;
    unsigned char* mask;
    uint16_t length;
    unsigned char indent;
    MimeRule() :range(1), indent(0) {}
};
class Mime {
public:
    string mimetype;
    vector<MimeRule> rules;
    int32_t priority;
};
class MimeEventAnalyzer::Private {
public:
    bool parsed;
    vector<Mime> mimes;
    Private() {}
    ~Private();
    void parseFile(const string&);
    void parseFiles();
};
MimeEventAnalyzer::Private::~Private() {
    vector<Mime>::const_iterator i;
    vector<MimeRule>::const_iterator j;
    for (i=mimes.begin(); i!=mimes.end(); ++i) {
        for (j=i->rules.begin(); j!=i->rules.end(); ++j) {
            free(j->value);
            if (j->mask) free(j->mask);
        }
    }
}
void
MimeEventAnalyzer::Private::parseFiles() {
    vector<string> files;
    files.push_back("/usr/share/mime/magic");

    vector<string>::const_iterator i;
    for (i = files.begin(); i< files.end(); ++i) {
        parseFile(*i);
    }
}

#ifndef __BIG_ENDIAN__
// endianness conversion is copied from kde's kmimetypefactory.cpp
#define MAKE_LITTLE_ENDIAN16(val) val = (uint16_t)(((uint16_t)(val) << 8)|((uint16_t)(val) >> 8))
#define MAKE_LITTLE_ENDIAN32(val) \
   val = (((uint32_t)(val) & 0xFF000000U) >> 24) | \
         (((uint32_t)(val) & 0x00FF0000U) >> 8) | \
         (((uint32_t)(val) & 0x0000FF00U) << 8) | \
         (((uint32_t)(val) & 0x000000FFU) << 24)
void
makeLittleEndian16(void* data, uint32_t len) {
    for (uint32_t i = 0; i < len; i += 2) {
       MAKE_LITTLE_ENDIAN16(*((uint16_t*)data+i));
    }
}
void
makeLittleEndian32(void* data, uint32_t len) {
    for (uint32_t i = 0; i < len; i += 4) {
       MAKE_LITTLE_ENDIAN32(*((uint32_t*)data+i));
    }
}
#endif

void
MimeEventAnalyzer::Private::parseFile(const string& file) {
    FileInputStream f(file.c_str());
    const char* data;
    int32_t nread = f.read(data, 12, 12);
    if (nread != 12 || memcmp(data, "MIME-Magic\0\n", 12) != 0) {
        // cannot read this magic file
        fprintf(stderr, "'%s' is not a valid magic file.\n", file.c_str());
        return;
    }
    while (true) {
        // find \n
        int64_t startpos = f.position();
        nread = f.read(data, 10000, 0);
        const char* end = data+nread;
        const char* pos = data;
        while (pos < end && *pos != '\n') pos++;
        if (pos >= end) {
            if (nread < -1) {
                fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
            }
            return;
        }
        const char* lpos = data;
        while (lpos < pos && *lpos != ':') lpos++;
        Mime mime;
        mime.mimetype.assign(lpos+1, pos-1);
        pos++;

        do {
            MimeRule rule;
            const char* lpos = pos;
            while (pos < end && isdigit(*pos)) pos++;
            if (pos >= end || *pos != '>') {
                fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
                return;
            }
            rule.indent = (unsigned char)atoi(lpos);
            lpos = ++pos;
            while (pos < end && isdigit(*pos)) pos++;
            if (pos >= end || *pos != '=') {
                fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
                return;
            }
            rule.offset = atol(lpos);
            lpos = ++pos;
            if (end-pos < 2) {
                fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
                return;
            }
            uint16_t len = readBigEndianUInt16(pos);
            rule.length = len;
            pos += 2;
            if (end-pos < len+1) {
                fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
                return;
            }
            rule.value = (unsigned char*)malloc(len);
            memcpy(rule.value, pos, len);
            pos += len;
            if (*pos == '&') {
                pos++;
                if (end-pos < len+1) {
                    fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
                    return;
                }
                rule.mask = (unsigned char*)malloc(len);
                memcpy(rule.mask, pos, len);
                pos += len;
            } else {
                rule.mask = 0;
            }
            if (*pos == '~') {
                lpos = ++pos;
                while (pos < end && isdigit(*pos)) pos++;
                if (pos >= end) {
                    fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
                    return;
                }
#ifndef __BIG_ENDIAN__
                char wordsize = (char)atoi(lpos);
                if (wordsize == 2) {
                    makeLittleEndian16(rule.value, len);
                    if (rule.mask) {
                        makeLittleEndian16(rule.mask, len);
                    }
                } else if (wordsize == 4) {
                    makeLittleEndian32(rule.value, len);
                    if (rule.mask) {
                        makeLittleEndian32(rule.mask, len);
                    }
                }
#endif
            }
            if (*pos == '+') {
                lpos = ++pos;
                while (pos < end && isdigit(*pos)) pos++;
                if (pos >= end) {
                    fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
                    return;
                }
                rule.range = atol(lpos);
            }
            if (*pos++ != '\n') {
                fprintf(stderr, "'%s' ended unexpectedly.\n", file.c_str());
                return;
            }
            mime.rules.push_back(rule);
        } while (pos < end && *pos != '[');
        f.reset(startpos + pos-data);
        mimes.push_back(mime);
    }
}

void
MimeEventAnalyzer::startAnalysis(AnalysisResult*) {
    p->parseFiles();
}
void
MimeEventAnalyzer::endAnalysis() {
}
void
MimeEventAnalyzer::handleData(const char* data, uint32_t length) {
}
bool
MimeEventAnalyzer::isReadyWithStream() {
    return true;
}
MimeEventAnalyzer::MimeEventAnalyzer(const MimeEventAnalyzerFactory*)
    :p(new Private()) {
}
MimeEventAnalyzer::~MimeEventAnalyzer() {
    delete p;
}
void
MimeEventAnalyzerFactory::registerFields(Strigi::FieldRegister& reg) {
    mimetypefield = reg.registerField("mimetype", FieldRegister::stringType, -1,
        0);
}

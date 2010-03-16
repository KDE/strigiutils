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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "oleendanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/oleinputstream.h>
#include <strigi/subinputstream.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/textutils.h>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cmath>

using namespace Strigi;
using namespace std;

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
     #define ICONV_CONST const
#else
     #define ICONV_CONST
#endif

WordText::WordText() :windows1252(iconv_open("UTF-8", "WINDOWS-1252")),
        utf16(iconv_open("UTF-8", "UTF-16")), out(0), len(0), capacity(0) {
}
WordText::~WordText() {
    if (out) free(out);
    iconv_close(windows1252);
    iconv_close(utf16);
}
void
WordText::addText(const char* d, size_t l) {
    // check if the block contains a '\0' character
    const char* zeroPtr = (const char*)memchr(d, 0, l);
    // if it does, check if the block is all 0's from there
    if (zeroPtr) {
        while (++zeroPtr < d+l && *zeroPtr == '\0') ;
    }
    if (zeroPtr && zeroPtr < d+l) {
        addText(d, l, utf16);
    } else {
        addText(d, l, windows1252);
    }
}
void
WordText::addText(const char* d, size_t l, iconv_t conv) {
    // try to add text from windows codepage 1252
    // we need free space 3x the length of the incoming string
    if (capacity-len < 3*l) {
        capacity = len + 3*l;
        out = (char*)realloc(out, capacity);
    }

    ICONV_CONST char* inbuf = (char*)d;
    size_t inbytesleft = l;
    char* outbuf = out + len;
    size_t outbytesleft = capacity - len;
    /*size_t r =*/ iconv(conv, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
     
    //fprintf(stderr, "%x %i %i added %i bytes %p\n", *d, r, inbytesleft,
    //    capacity-outbytesleft-len, conv);
    len += capacity - len - outbytesleft;
    out[len] = '\0';
}
void
WordText::cleanText() {
    char* c = out;
    char* end = out+len;
    while (c < end && *c) {
        int cvalue = *c;
        switch (cvalue) {
        case 1: // ?
        case 2: // ?
        case 7: // cell/row end
        case 8: // ?
        case 11: // vertical tab?
        case 12: // Page break / Section mark
        case 13: // Paragraph end
        case 14: // Column end
        case 19: // Field start
        case 20: // ?
            *c = '\n';
	    break;
        case 21: // Field end
            *c = ' '; // removing it would be better?
	    break;
        case 30: // Non-breakn hyphen
            *c = '-';
            break;
        case 31: // Non-required hyphen
            *c = '-';
	    break;
        case 160: // Non-breaking space
            *c = ' ';
            break;
        case 194: // beats me, might be artifact from decoding windows 1252
            *c = ' ';
        case 0xa7: // ?
        case 3: // ? but should be removed, not replaced
        case 4: // ? but should be removed, not replaced
        case 5: // ? but should be removed, not replaced
            *c = ' ';
        default:;
        }
        c++;
    }
    len = c - out;
}
void
OleEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    static const unsigned char summaryKey[] = {
        0xE0,0x85,0x9F,0xF2,0xF9,0x4F,0x68,0x10,
        0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9};
    static const unsigned char docSummaryKey[]= {
        0x02,0xD5,0xCD,0xD5,0x9C,0x2E,0x1B,0x10,
        0x93,0x97,0x08,0x00,0x2B,0x2C,0xF9,0xAE};
    string key;
    key.assign((const char*)summaryKey, 16);
    map<int,const RegisteredField*>* m = &fieldsMaps[key];

    // register the fields for the Summary Information Stream
    (*m)[2] = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#title");
    addField((*m)[2]);
    (*m)[3] = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#subject");
    addField((*m)[3]);
    /* //FIXME: either get rid of this or replace with NIE equivalent
    (*m)[4] = reg.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#author");
    addField((*m)[4]);
    */
    (*m)[5] = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#keyword");
    addField((*m)[5]);
    (*m)[6] = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#comment");
    addField((*m)[6]);

    // register the fields for the Document Summary Information Stream
    key.assign((const char*)docSummaryKey, 16);
    m = &fieldsMaps[key];
    /* //FIXME: either get rid of this or replace with NIE equivalent
    (*m)[2] = reg.registerField("http://strigi.sf.net/ontologies/homeless#oleCategory");
    addField((*m)[2]);
    (*m)[3] = reg.registerField("http://strigi.sf.net/ontologies/homeless#olePresentationTarget");
    addField((*m)[3]);
    (*m)[14] = reg.registerField("http://strigi.sf.net/ontologies/homeless#oleManager");
    addField((*m)[14]);
    (*m)[15] = reg.registerField("http://strigi.sf.net/ontologies/homeless#oleCompany");
    addField((*m)[15]);
    */
    typeField = reg.typeField;
    addField(typeField);
}
const map<int, const RegisteredField*>*
OleEndAnalyzerFactory::getFieldMap(const string& key) const {
    map<string, map<int,const RegisteredField*> >::const_iterator i
        = fieldsMaps.find(key);
    return (i == fieldsMaps.end()) ?0 :&i->second;
}
// parse with info from
// http://www.wotsit.org/getfile.asp?file=wword8&sc=230027800
bool
OleEndAnalyzer::tryFIB(AnalysisResult& ar, InputStream* in) {
    const char* d;
    int32_t size = 426;
    int32_t nread = in->read(d, size, size);
    in->reset(0);
    if (nread != size
            || (unsigned char)d[0] != 0xec || (unsigned char)d[1] != 0xa5) {
        return false;
    }
    bool complex = (d[10] & 4) == 4;
    if (complex) return false;
    int32_t fcMin = readLittleEndianInt32(d+24);
    int32_t fcMac = readLittleEndianInt32(d+28);

    // for some reason we sometimes need to add 512 here. No clue why.
    // if the first 512 bytes are 0 we do this
    size = fcMin+512;
    nread = in->read(d, size, size);
    in->reset(0);
    if (nread != size) {
        return false;
    }
    int i;
    for (i=0; i<512 && d[i+fcMin] == 0; i++) ;
    if (i == 512) {
        fcMin += 512;
        fcMac += 512;
    }

    size = fcMac;
    nread = in->read(d, size, size);
    in->reset(0);
    if (nread != size) {
        return false;
    }

    wordtext.reset();
    for (int32_t dp = fcMin; dp < fcMac; dp += size) {
        size = fcMac-dp;
        if (size > 512) size = 512;
        wordtext.addText(d+dp, size);
    }
    wordtext.cleanText();
    ar.addText(wordtext.text(), (int32_t)wordtext.length());
    wordtext.reset();
    return true;
}
bool
OleEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return OleInputStream::checkHeader(header, headersize);
}
bool
tryThumbsdbEntry(const string& name, AnalysisResult& ar, InputStream* in) {
    static const char magic[] = {0x0c, 0, 0, 0, 0x01, 0, 0, 0};
    const char* d;
    uint32_t nread = in->read(d, 12, 12);
    if (nread != 12 || memcmp(magic, d, 8)) {
        in->reset(0);
        return false;
    }
    SubInputStream thumb(in, in->size()-12);
    ar.indexChild(name, 0, &thumb);
    return true;
}
/**
 * Extract images from a 'Pictures' field from a ppt file.
 * http://jakarta.apache.org/poi/apidocs/org/apache/poi/hslf/model/Picture.html
 **/
void
tryPictures(AnalysisResult& ar, InputStream* in) {
    const char* d;
    int32_t nread = in->read(d, 25, 25);
    ostringstream s;
    int pos = 1;
    while (nread == 25) {
        uint32_t size = readLittleEndianInt32(d+4)-17;
        SubInputStream sub(in, size);
        s << "Pictures/" << pos++;
        ar.indexChild(s.str(), 0, &sub);
        const char* dummy;
        while (sub.read(dummy, 1, 0) > 0) {
            // skip to the end
        }
        s.str("");
        nread = in->read(d, 25, 25);
    }
}
// format description: http://jakarta.apache.org/poi/hpsf/internals.html
bool
OleEndAnalyzer::tryPropertyStream(AnalysisResult& idx,
        InputStream* in) {
    static const unsigned char magic[] = {0xfe, 0xff, 0, 0};
    const char* d;
    uint32_t nread = in->read(d, 28, 28);
    in->reset(0);
    if (nread != 28 || memcmp(magic, d, 4)) {
        return false;
    }
    // read all the data
    nread = in->read(d, (uint32_t)in->size(), (uint32_t)in->size());
    if (nread != in->size()) {
        return false;
    }
    int32_t n = readLittleEndianUInt32(d+24);
    if (in->size() < 28+n*20) {
        return false;
    }
    for (int32_t i=0; i<n; ++i) {
        const char* key = d + 28 + i*20;
        int32_t offset = readLittleEndianInt32(key+16);
        if (offset >= in->size()) {
            return false;
        }
        handlePropertyStream(key, d+offset, d+in->size());
    }
    return true;
}
void
OleEndAnalyzer::handleProperty(AnalysisResult* result,
        const RegisteredField* field, const char* data, const char* end) {
    if (end-data < 8) {
        return;
    }
    int32_t datatype = readLittleEndianInt32(data);
    if (datatype == 30) {
        int32_t len = readLittleEndianInt32(data+4);
        data += 8;
        if (len > 0 && len <= end-data) {
            // remove trailing '\0' characters
            while (len > 0 && data[len-1] == 0) len--;
            // report the field
            result->addValue(field, data, len);
        }
    }
}
void
OleEndAnalyzer::handlePropertyStream(const char* key, const char* data,
        const char* end) {
    // get the fieldtable
    string k(key, 16);
    const map<int, const RegisteredField*>* table = factory->getFieldMap(k);
    if (table == 0) {
        return;
    }

    int32_t len = readLittleEndianInt32(data);
    const char* p = data + 8;
    const char* n = data + readLittleEndianInt32(data+4)*4 + 8;
    if (len < 0 || (len > end-data) || n > end) {
        return;
    }
    map<int, const RegisteredField*>::const_iterator field;
    while (p < n) {
        int32_t id = readLittleEndianInt32(p);
        field = table->find(id);
        if (field != table->end()) {
            int32_t offset = readLittleEndianInt32(p+4);
            if (offset > 0) {
                handleProperty(result, field->second, data+offset, end);
            }
        }
        p += 8;
    }
}
/**
 * Store the table stream for later reference when parsing the text stream.
 **/
string
OleEndAnalyzer::getStreamString(InputStream* in) const {
    const char* d;
    int32_t n = 512;
    int32_t m = in->read(d, n+1, 0);
    in->reset(0);
    while (m > n) {
        n = m;
        m = in->read(d, n+1, 0);
        in->reset(0);
    }
    if (m > 0) {
        return string(d, m);
    }
    return string();
}
signed char
OleEndAnalyzer::analyze(AnalysisResult& ar, InputStream* in) {
    if(!in)
        return -1;

    result = &ar;
    OleInputStream ole(in);
    InputStream *s = ole.nextEntry();
    if (ole.status()) {
        fprintf(stderr, "error: %s\n", ole.error());
	return -1;
    }
    while (s) {
        string name = ole.entryInfo().filename;
        if (name.size()) {
            char first = name[0];
            if (first < 10) {
                name = name.substr(1);
            }
	    if (tryFIB(ar, s)) {
            } else if (tryThumbsdbEntry(name, ar, s)) {
            } else if (first == 5) {
                // todo: handle property stream
                tryPropertyStream(ar, s);
            } else if (name == "Pictures") {
                tryPictures(ar, s);
            //} else if (name == "1Table" || name == "0Table") {
            //    word1Table.assign(getStreamString(s));
            } else {
                ar.indexChild(name, ole.entryInfo().mtime, s);
            }
        }
        s = ole.nextEntry();
    }
    if (ole.status() == Error) {
        m_error = ole.error();
        return -1;
    } else {
        ar.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Document");
        m_error.resize(0);
    }
    return 0;
}

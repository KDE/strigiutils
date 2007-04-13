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
#include "strigiconfig.h"
#include "oleinputstream.h"
#include "oleendanalyzer.h"
#include "subinputstream.h"
#include "analysisresult.h"
#include "fieldtypes.h"
#include "textutils.h"
#include <sstream>
using namespace Strigi;
using namespace std;

void
OleEndAnalyzerFactory::registerFields(FieldRegister& reg) {
    static const char summaryKey[] = {0xE0,0x85,0x9F,0xF2,0xF9,0x4F,0x68,0x10,
        0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9};
    static const char docSummaryKey[]= {0x02,0xD5,0xCD,0xD5,0x9C,0x2E,0x1B,0x10,
        0x93,0x97,0x08,0x00,0x2B,0x2C,0xF9,0xAE};
    const RegisteredField* r;
    string key;
    map<int,const RegisteredField*>* m;

    // register the fields for the Summary Information Stream
    key.assign(summaryKey, 16);
    m = &fieldsMaps[key];
    r = reg.registerField("title", FieldRegister::stringType, 1, 0);
    if (r) (*m)[2] = r;
    r = reg.registerField("subject", FieldRegister::stringType, -1, 0);
    if (r) (*m)[3] = r;
    r = reg.registerField("author", FieldRegister::stringType, -1, 0);
    if (r) (*m)[4] = r;
    r = reg.registerField("keywords", FieldRegister::stringType, -1, 0);
    if (r) (*m)[5] = r;
    r = reg.registerField("comments", FieldRegister::stringType, -1, 0);
    if (r) (*m)[6] = r;

    // register the fields for the Document Summary Information Stream
    key.assign(docSummaryKey, 16);
    m = &fieldsMaps[key];
    r = reg.registerField("category", FieldRegister::stringType, 1, 0);
    if (r) (*m)[2] = r;
    r = reg.registerField("presentationtarget",FieldRegister::stringType, 1, 0);
    if (r) (*m)[3] = r;
    r = reg.registerField("manager", FieldRegister::stringType, 1, 0);
    if (r) (*m)[14] = r;
    r = reg.registerField("company", FieldRegister::stringType, 1, 0);
    if (r) (*m)[15] = r;
}
const map<int, const RegisteredField*>*
OleEndAnalyzerFactory::getFieldMap(const string& key) const {
    map<string, map<int,const RegisteredField*> >::const_iterator i
        = fieldsMaps.find(key);
    return (i == fieldsMaps.end()) ?0 :&i->second;
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
        s.str("");
        nread = in->read(d, 25, 25);
    }
}
// format description: http://jakarta.apache.org/poi/hpsf/internals.html
bool
OleEndAnalyzer::tryPropertyStream(AnalysisResult& idx,
        InputStream* in) {
    static const char magic[] = {0xfe, 0xff, 0, 0};
    const char* d;
    uint32_t nread = in->read(d, 28, 28);
    in->reset(0);
    if (nread != 28 || memcmp(magic, d, 4)) {
        return false;
    }
    // read all the data
    nread = in->read(d, in->size(), in->size());
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
OleEndAnalyzer::handleProperty(const RegisteredField* field, const char* data) {
    int32_t datatype = readLittleEndianInt32(data);
    // currently we only support null-terminated strings
    if (datatype == 30) {
        result->addValue(field, data+4);
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
    if (len < 0 || len > end-data || n > end) {
        return;
    }
    map<int, const RegisteredField*>::const_iterator field;
    while (p < n) {
        int32_t id = readLittleEndianInt32(p);
        field = table->find(id);
        if (field != table->end()) {
            int32_t offset = readLittleEndianInt32(p+4);
            handleProperty(field->second, data+offset);
        }
        p += 8;
    }
}
char
OleEndAnalyzer::analyze(AnalysisResult& ar, InputStream* in) {
    if(!in)
        return -1;

    result = &ar;
    OleInputStream ole(in);
    InputStream *s = ole.nextEntry();
    if (ole.status()) {
        fprintf(stderr, "error: %s\n", ole.error());
//        exit(1);
    }
    while (s) {
        const string& name = ole.entryInfo().filename;
        if (name.size()) {
            if (tryThumbsdbEntry(name, ar, s)) {
            } else if (name[0] == 5) {
                // todo: handle property stream
                tryPropertyStream(ar, s);
            } else if (name == "Pictures") {
                tryPictures(ar, s);
            } else {
                ar.indexChild(name, ole.entryInfo().mtime,
                    s);
            }
        }
        s = ole.nextEntry();
    }
    if (ole.status() == Error) {
        m_error = ole.error();
        return -1;
    } else {
        m_error.resize(0);
    }
    return 0;
}

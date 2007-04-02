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
#include "strigiconfig.h"
#include "analysisresult.h"
#include "indexwriter.h"
#include "textutils.h"
using namespace Strigi;
using namespace std;
string
AnalysisResult::extension() const {
    string::size_type p1 = m_name.rfind('.');
    string::size_type p2 = m_name.rfind('/');
    if (p1 != string::npos && (p2 == string::npos || p1 > p2)) {
        return m_name.substr(p1+1);
    }
    return "";
}
void
AnalysisResult::addValue(const RegisteredField* field, const std::string& value){
    // make sure only utf8 is stored
    if (!checkUtf8(value)) {
        fprintf(stderr, "'%s' is not a UTF8 string\n", value.c_str());
        return;
    }
    m_writer.addField(this, field, value);
}
void
AnalysisResult::addValue(const RegisteredField* field,
        const char* data, uint32_t length) {
    m_writer.addField(this, field, (const unsigned char*)data, length);
}
void
AnalysisResult::addValue(const RegisteredField* field, int32_t value) {
    m_writer.addField(this, field, value);
}
void
AnalysisResult::addValue(const RegisteredField* field, uint32_t value) {
    m_writer.addField(this, field, value);
}
void
AnalysisResult::addValue(const RegisteredField* field, double value) {
    m_writer.addField(this, field, value);
}
void
AnalysisResult::addValue(RegisteredField*field, const std::string&name,
        const std::string&value) {
}

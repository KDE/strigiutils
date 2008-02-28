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
#include "mailendanalyzer.h"
#include <strigi/strigiconfig.h>
#include "mailinputstream.h"
#include "encodinginputstream.h"
#include "analysisresult.h"
#include "textendanalyzer.h"
#include "fieldtypes.h"
#include <iostream>
using namespace Strigi;
using namespace std;

void
MailEndAnalyzerFactory::registerFields(FieldRegister& r) {
    titleField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#subject");
    contenttypeField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#contentType");
    fromField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#author");
    toField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#to");
    ccField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#cc");
    bccField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#bcc");
    contentidField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#id");
    contentlinkField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#links");
    emailInReplyToField = r.registerField(
        "http://freedesktop.org/standards/xesam/1.0/core#inReplyTo");
    typeField = r.typeField;

    addField(titleField);
    addField(contenttypeField);
    addField(fromField);
    addField(toField);
    addField(ccField);
    addField(bccField);
    addField(contentidField);
    addField(contentlinkField);
    addField(emailInReplyToField);
    addField(typeField);
}

bool
MailEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return MailInputStream::checkHeader(header, headersize);
}
string
charset(const string& contenttype) {
    const char* s = strstr(contenttype.c_str(), "charset");
    if (s) {
        char c = s[8];
        if (c == '\'' || c == '"') {
            s += 9;
            const char* e = strchr(s, c);
            if (e) {
                return string(s, e-s);
            }
        }
    }
    return string();
}
char
MailEndAnalyzer::analyze(AnalysisResult& idx, InputStream* in) {
    if(!in)
        return -1;

    MailInputStream mail(in);
    InputStream *s = mail.nextEntry();
    if (mail.status() == Error) {
        m_error = mail.error();
        return -1;
    }
    string enc(charset(mail.contentType()));
    if (enc.length()) {
        idx.setEncoding(enc.c_str());
    }
    idx.addValue(factory->typeField, "http://freedesktop.org/standards/xesam/1.0/core#Email");
    idx.addValue(factory->titleField, mail.subject());
    idx.addValue(factory->contenttypeField, mail.contentType());
    idx.addValue(factory->fromField, mail.from());
    idx.addValue(factory->toField, mail.to());
    if (mail.cc().length() > 0) idx.addValue(factory->ccField, mail.cc());
    if (mail.bcc().length() > 0) idx.addValue(factory->bccField, mail.bcc());
    if (mail.messageid().length() > 0)
        idx.addValue(factory->contentidField, mail.messageid());
    if (mail.inreplyto().length() > 0)
        idx.addValue(factory->emailInReplyToField, mail.inreplyto());
    if (mail.references().length() > 0)
        idx.addValue(factory->contentlinkField, mail.references());
    if (s != 0) {
        TextEndAnalyzer tea;
        if (enc.length()) {
            EncodingInputStream eis(s, enc.c_str());
            tea.analyze(idx, &eis);
        } else {
            tea.analyze(idx, s);
        }
    }
    s = mail.nextEntry();
    int n = 1;
    while (s) {
        std::string file;
        if (mail.entryInfo().filename.length() == 0) {
            file = (char)(n+'1');
        } else {
            file = mail.entryInfo().filename;
        }
        // maybe use the date of sending the mail here
        idx.indexChild(file, idx.mTime(), s);
        s = mail.nextEntry();
        n++;
    }
    if (mail.status() == Error) {
        m_error = mail.error();
        return -1;
    } else {
        m_error.resize(0);
    }
    return 0;
}


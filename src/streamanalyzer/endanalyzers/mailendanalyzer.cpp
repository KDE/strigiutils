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
#include "strigiconfig.h"
#include "mailinputstream.h"
#include "analysisresult.h"
#include "textendanalyzer.h"
#include "fieldtypes.h"
using namespace Strigi;
using namespace std;

const string MailEndAnalyzerFactory::titleFieldName = "email.subject";
const string MailEndAnalyzerFactory::contenttypeFieldName = "email.content_type";
const string MailEndAnalyzerFactory::fromFieldName = "email.from";
const string MailEndAnalyzerFactory::toFieldName = "email.to";

void
MailEndAnalyzerFactory::registerFields(FieldRegister& r) {
    titleField
        = r.registerField(titleFieldName, FieldRegister::stringType, 1, 0);
    contenttypeField = r.registerField(contenttypeFieldName,
        FieldRegister::stringType, 1, 0);
    fromField = r.registerField(fromFieldName, FieldRegister::stringType, 1, 0);
    toField = r.registerField(toFieldName, FieldRegister::stringType, 1, 0);
}

bool
MailEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return MailInputStream::checkHeader(header, headersize);
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
/*    if (s == 0) {
        m_error = "mail contains no body";
        return -1;
    }*/
    idx.addValue(factory->titleField, mail.subject());
    idx.addValue(factory->contenttypeField, mail.contentType());
    idx.addValue(factory->fromField, mail.from());
    idx.addValue(factory->toField, mail.to());
    TextEndAnalyzer tea;
    if (s != 0 && tea.analyze(idx, s) != 0) {
        m_error = "Error reading mail body.";
        return -1;
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


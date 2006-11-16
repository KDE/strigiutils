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
#include "mailendanalyzer.h"
#include "mailinputstream.h"
#include "indexable.h"
#include "textendanalyzer.h"
using namespace jstreams;

bool
MailEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    return MailInputStream::checkHeader(header, headersize);
}
char
MailEndAnalyzer::analyze(jstreams::Indexable& idx, jstreams::InputStream* in) {
    MailInputStream mail(in);
    InputStream *s = mail.nextEntry();
    if (mail.getStatus() == jstreams::Error) {
        error = mail.getError();
        return -1;
    }
    if (s == 0) {
        error = "mail contains no body";
        return -1;
    }
    idx.setField("title", mail.getSubject());
    idx.setField("contenttype", mail.getContentType());
    TextEndAnalyzer tea;
    if (tea.analyze(idx, s) != 0) {
        error = "Error reading mail body.";
        return -1;
    }
    s = mail.nextEntry();
    int n = 1;
    while (s) {
        std::string file;
        if (mail.getEntryInfo().filename.length() == 0) {
            file = (char)(n+'1');
        } else {
            file = mail.getEntryInfo().filename;
        }
        // maybe use the date of sending the mail here
        idx.indexChild(file, idx.getMTime(), *s);
        s = mail.nextEntry();
        n++;
    }
    if (mail.getStatus() == jstreams::Error) {
        error = mail.getError();
        return -1;
    } else {
        error.resize(0);
    }
    return 0;
}


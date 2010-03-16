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
#include <strigi/mailinputstream.h>
#include <strigi/encodinginputstream.h>
#include <strigi/analysisresult.h>
#include "textendanalyzer.h"
#include <strigi/fieldtypes.h>
#include <iostream>
using namespace Strigi;
using namespace std;

#define NMO_PROPOSAL "http://www.semanticdesktop.org/ontologies/nmo#"

const string
    titleFieldName(
        "http://www.semanticdesktop.org/ontologies/2007/01/19/nie#subject"),
    fromFieldName(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#from"),
    toFieldName(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#to"),
    ccFieldName(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#cc"),
    bccFieldName(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#bcc"),
    contentidFieldName(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#messageId"),
    contentlinkFieldName(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#references"),
    emailInReplyToFieldName(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#inReplyTo"),

    typeFieldName(
	"http://www.w3.org/1999/02/22-rdf-syntax-ns#type"),
    fullnameFieldName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#fullname"),
    hasEmailAddressFieldName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#hasEmailAddress"),
    emailAddressFieldName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#emailAddress"),

    emailClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nmo#Email"),
    contactClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#Contact"),
    emailAddressClassName(
	"http://www.semanticdesktop.org/ontologies/2007/03/22/nco#EmailAddress"),
    mimePartClassName(
	NMO_PROPOSAL "MimePart");

#undef NMO_PROPOSAL

void
MailEndAnalyzerFactory::registerFields(FieldRegister& r) {
    titleField = r.registerField(titleFieldName);
    contenttypeField = r.mimetypeField;
    fromField = r.registerField(fromFieldName);
    toField = r.registerField(toFieldName);
    ccField = r.registerField(ccFieldName);
    bccField = r.registerField(bccFieldName);
    contentidField = r.registerField(contentidFieldName);
    contentlinkField = r.registerField(contentlinkFieldName);
    emailInReplyToField = r.registerField(emailInReplyToFieldName);
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
void
splitAddress(const string& addr, string& name, string& email) {
  size_t p;
  if( (p = addr.find("<"))!= string::npos ) {
    name = addr.substr(0, p);
    email = addr.substr(p+1, addr.rfind(">") -p -1);
  } else {
    name = "";
    email = addr;
  }
}
string 
processAddress(Strigi::AnalysisResult& idx, const string& address) {
    string uri(idx.newAnonymousUri());
    string cmUri;
    string name, email;

    splitAddress(address, name, email);
    cmUri = "mailto:" + email;

    idx.addTriplet(uri, typeFieldName, contactClassName);
    if (name.size())
	idx.addTriplet(uri, fullnameFieldName, name);
    idx.addTriplet(uri, hasEmailAddressFieldName, cmUri);
    idx.addTriplet(cmUri, typeFieldName, emailAddressClassName);
    idx.addTriplet(cmUri, emailAddressFieldName, email);
    
    return uri;
}
signed char
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
    idx.addValue(factory->typeField, emailClassName);
    idx.addValue(factory->titleField, mail.subject());
    idx.addValue(factory->contenttypeField, mail.contentType());
    
    idx.addValue(factory->fromField, processAddress(idx, mail.from()) );
    
    idx.addValue(factory->toField, processAddress(idx, mail.to()) );
    if (mail.cc().length() > 0) idx.addValue(factory->ccField, processAddress(idx, mail.cc()) );
    if (mail.bcc().length() > 0) idx.addValue(factory->bccField, processAddress(idx, mail.bcc()) );
    if (mail.messageid().length() > 0)
        idx.addValue(factory->contentidField, mail.messageid());
    if (mail.inreplyto().length() > 0) {
	string uri(idx.newAnonymousUri());
        idx.addValue(factory->emailInReplyToField, uri);
	idx.addTriplet(uri, typeFieldName, emailClassName);
	idx.addTriplet(uri, contentidFieldName, mail.inreplyto());
    }
    if (mail.references().length() > 0) {
	string uri(idx.newAnonymousUri());
	idx.addValue(factory->contentlinkField, uri);
	idx.addTriplet(uri, typeFieldName, emailClassName);
	idx.addTriplet(uri, contentidFieldName, mail.references());
    }
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
        if (idx.child()) {
            idx.child()->addValue(factory->typeField, mimePartClassName);
        }

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


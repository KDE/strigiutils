/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Arend van Beelen jr. <arend@auton.nl>
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

#include "odfmimetypelineanalyzer.h"
#include <cassert>
#include <cstring>

#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>

using namespace Strigi;

void
OdfMimeTypeLineAnalyzerFactory::registerFields(FieldRegister &reg) {
    mimeTypeField = reg.mimetypeField;
    typeField = reg.typeField;
    addField(mimeTypeField);
    addField(typeField);
}

Strigi::StreamLineAnalyzer *
OdfMimeTypeLineAnalyzerFactory::newInstance() const {
    return new OdfMimeTypeLineAnalyzer(this);
}

OdfMimeTypeLineAnalyzer::OdfMimeTypeLineAnalyzer(
        const OdfMimeTypeLineAnalyzerFactory *factory) :
    m_factory(factory),
    m_ready(true),
    m_result(0) {
    assert(m_factory != 0);
}

void
OdfMimeTypeLineAnalyzer::startAnalysis(AnalysisResult *result) {
    assert(result != 0);

    if(result->fileName() == "mimetype" && result->parent() != 0
            && result->parent()->mimeType() == "application/zip") {
        m_result = result->parent();
        m_ready = false;
    } else {
        m_ready = true;
    }
}

void
OdfMimeTypeLineAnalyzer::endAnalysis(bool /*complete*/) {
    m_result = 0;
}

void
OdfMimeTypeLineAnalyzer::handleLine(const char *data, uint32_t length) {
    assert(m_result != 0);

    if (length < 35 || std::strncmp(data,
            "application/vnd.oasis.opendocument.", 35) != 0) {
        m_ready = true;
        return;
    }
/*
application/vnd.oasis.opendocument.text  odt
application/vnd.oasis.opendocument.text-template  ott
application/vnd.oasis.opendocument.text-master  odm
application/vnd.oasis.opendocument.text-web  oth
application/vnd.oasis.opendocument.graphics  odg
application/vnd.oasis.opendocument.graphics-template  otg
application/vnd.oasis.opendocument.presentation  odp
application/vnd.oasis.opendocument.presentation-template  otp
application/vnd.oasis.opendocument.spreadsheet  ods
application/vnd.oasis.opendocument.spreadsheet-template  ots
application/vnd.oasis.opendocument.chart  odc
application/vnd.oasis.opendocument.chart-template  otc
application/vnd.oasis.opendocument.image  odi
application/vnd.oasis.opendocument.image-template  oti
application/vnd.oasis.opendocument.formula  odf
application/vnd.oasis.opendocument.formula-template  otf
*/

    const char *rdftype = NULL;
    if( length >= (35+4) && std::strncmp(data+35, "text", 4) == 0 ) {
        rdftype = "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#TextDocument";
    } else if ( length >= (35+12) && std::strncmp(data+35, "presentation", 12) == 0 ) {
        rdftype = "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Presentation";
    } else if ( length >= (35+11) && std::strncmp(data+35, "spreadsheet", 11) == 0 ) {
        rdftype = "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Spreadsheet";
    }

    if(rdftype) {
        m_result->addValue(m_factory->typeField, rdftype);
    }

    std::string mimeType;
    mimeType.assign(data, length);

    m_result->addValue(m_factory->mimeTypeField, mimeType);
    m_result->setMimeType(mimeType);
}

bool OdfMimeTypeLineAnalyzer::isReadyWithStream() {
    return m_ready;
}

/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Arend van Beelen jr. <www.arendjr.nl>
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

#include <cassert>
#include <cstring>
#include <string>

#include "analysisresult.h"
#include "fieldtypes.h"
#include "odfsaxanalyzer.h"

using namespace Strigi;

static const char *dcNS = "http://purl.org/dc/elements/1.1/";
static const char *metaNS = "urn:oasis:names:tc:opendocument:xmlns:meta:1.0";
//static const char *officeNS = "urn:oasis:names:tc:opendocument:xmlns:office:1.0";
//static const char *svgNS = "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0";
//static const char *textNS = "urn:oasis:names:tc:opendocument:xmlns:text:1.0";

void OdfSaxAnalyzerFactory::registerFields(FieldRegister &reg) {
    creatorField = reg.registerField("content.authors", FieldRegister::stringType, 1, 0);
    creationTimeField = reg.registerField("content.creation_time", FieldRegister::stringType, 1, 0);
    titleField = reg.registerField("content.title", FieldRegister::stringType, 1, 0);
    subjectField = reg.registerField("content.subject", FieldRegister::stringType, 1, 0);
    descriptionField = reg.registerField("content.description", FieldRegister::stringType, 1, 0);
    languageField = reg.registerField("content.language", FieldRegister::stringType, 1, 0);
    keywordField = reg.registerField("user.categories", FieldRegister::stringType, 1, 0);
}

Strigi::StreamSaxAnalyzer *OdfSaxAnalyzerFactory::newInstance() const {
    return new OdfSaxAnalyzer(this);
}

OdfSaxAnalyzer::OdfSaxAnalyzer(const OdfSaxAnalyzerFactory *factory) :
  m_factory(factory),
  m_result(0),
  m_currentField(0) {
    assert(m_factory != 0);
}

void OdfSaxAnalyzer::startAnalysis(AnalysisResult *result) {
    assert(result != 0);

    if(result->parent() != 0) {
        m_result = result->parent();
    } else {
        m_result = result;
    }
}

void OdfSaxAnalyzer::endAnalysis() {
    m_result = 0;
}

void OdfSaxAnalyzer::startElement(const char *localname, const char *prefix,
                                  const char *uri, int nb_namespaces, const char **namespaces,
                                  int nb_attributes, int nb_defaulted, const char **attributes) {
    assert(m_result != 0);

    if(uri && strcmp(uri, dcNS) == 0) {
        if(strcmp(localname, "creator") == 0) {
            m_currentField = m_factory->creatorField;
        } else if(strcmp(localname, "title") == 0) {
            m_currentField = m_factory->titleField;
        } else if(strcmp(localname, "subject") == 0) {
            m_currentField = m_factory->subjectField;
        } else if(strcmp(localname, "description") == 0) {
            m_currentField = m_factory->descriptionField;
        } else if(strcmp(localname, "language") == 0) {
            m_currentField = m_factory->languageField;
        }
    } else if(uri && strcmp(uri, metaNS) == 0) {
        if(strcmp(localname, "creation-date") == 0) {
            m_currentField = m_factory->creationTimeField;
        } else if(strcmp(localname, "keyword") == 0) {
            m_currentField = m_factory->keywordField;
        }
    }
}

void OdfSaxAnalyzer::endElement(const char *localname, const char *prefix,
                                const char *uri) {
    m_currentField = 0;
}

void OdfSaxAnalyzer::characters(const char *data, uint32_t length) {
    assert(m_result != 0);

    if(m_currentField != 0) {
        m_result->addValue(m_currentField, data, length);
    }
}

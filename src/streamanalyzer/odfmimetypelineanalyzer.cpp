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

#include "analysisresult.h"
#include "fieldtypes.h"
#include "odfmimetypelineanalyzer.h"

using namespace Strigi;

void OdfMimeTypeLineAnalyzerFactory::registerFields(FieldRegister &reg) {
    mimeTypeField = reg.registerField("content.mime_type", FieldRegister::stringType, 1, 0);
}

Strigi::StreamLineAnalyzer *OdfMimeTypeLineAnalyzerFactory::newInstance() const {
    return new OdfMimeTypeLineAnalyzer(this);
}

OdfMimeTypeLineAnalyzer::OdfMimeTypeLineAnalyzer(const OdfMimeTypeLineAnalyzerFactory *factory) :
  m_factory(factory),
  m_ready(true),
  m_result(0) {
    assert(m_factory != 0);
}

void OdfMimeTypeLineAnalyzer::startAnalysis(AnalysisResult *result) {
    assert(result != 0);

    if(result->fileName() == "mimetype" &&
       result->parent() != 0 && result->parent()->mimeType() == "application/zip") {
        m_result = result->parent();
        m_ready = false;
    } else {
        m_ready = true;
    }
}

void OdfMimeTypeLineAnalyzer::endAnalysis() {
    m_result = 0;
}

void OdfMimeTypeLineAnalyzer::handleLine(const char *data, uint32_t length) {
    assert(m_result != 0);

    if(length < 35 || strncmp(data, "application/vnd.oasis.opendocument.", 35) != 0) {
        m_ready = true;
        return;
    }

    std::string mimeType;
    mimeType.assign(data, length);

    m_result->setField(m_factory->mimeTypeField, mimeType);
    m_result->setMimeType(mimeType);
}

bool OdfMimeTypeLineAnalyzer::isReadyWithStream() {
    return m_ready;
}

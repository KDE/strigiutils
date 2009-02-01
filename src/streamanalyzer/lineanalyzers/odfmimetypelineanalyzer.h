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

#ifndef ODFMIMETYPELINEANALYZER_H
#define ODFMIMETYPELINEANALYZER_H

#include "streamlineanalyzer.h"

class OdfMimeTypeLineAnalyzerFactory : public Strigi::StreamLineAnalyzerFactory {
public:
    const Strigi::RegisteredField *mimeTypeField;
    const Strigi::RegisteredField *typeField;

    const char *name() const {
        return "OdfMimeTypeLineAnalyzer";
    }

    void registerFields(Strigi::FieldRegister &);

    Strigi::StreamLineAnalyzer *newInstance() const;
};

class OdfMimeTypeLineAnalyzer : public Strigi::StreamLineAnalyzer {
public:
    OdfMimeTypeLineAnalyzer(const OdfMimeTypeLineAnalyzerFactory *factory);

    const char* name() const {
        return "OdfMimeTypeLineAnalyzer";
    }

    void startAnalysis(Strigi::AnalysisResult *result);
    void endAnalysis(bool complete);
    void handleLine(const char *data, uint32_t length);
    bool isReadyWithStream();

private:
    const OdfMimeTypeLineAnalyzerFactory *m_factory;

    bool m_ready;
    Strigi::AnalysisResult *m_result;
};

#endif // ODFMIMETYPELINEANALYZER_H

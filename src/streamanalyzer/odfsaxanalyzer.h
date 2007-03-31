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

#ifndef ODFSAXANALYZER_H
#define ODFSAXANALYZER_H

#include "streamsaxanalyzer.h"

class OdfSaxAnalyzerFactory : public Strigi::StreamSaxAnalyzerFactory {
public:
    const Strigi::RegisteredField *creatorField;
    const Strigi::RegisteredField *creationTimeField;
    const Strigi::RegisteredField *titleField;
    const Strigi::RegisteredField *subjectField;
    const Strigi::RegisteredField *descriptionField;
    const Strigi::RegisteredField *languageField;
    const Strigi::RegisteredField *keywordField;

    const char *name() const {
        return "OdfSaxAnalyzerFactory";
    }

    void registerFields(Strigi::FieldRegister &);

    Strigi::StreamSaxAnalyzer *newInstance() const;
};

class OdfSaxAnalyzer : public Strigi::StreamSaxAnalyzer {
public:
    OdfSaxAnalyzer(const OdfSaxAnalyzerFactory *factory);

    const char* name() const {
        return "OdfSaxAnalyzer";
    }

    void startAnalysis(Strigi::AnalysisResult *result);
    void endAnalysis();
    void startElement(const char *localname, const char *prefix,
                      const char *uri, int nb_namespaces, const char **namespaces,
                      int nb_attributes, int nb_defaulted, const char **attributes);
    void endElement(const char *localname, const char *prefix,
                    const char *uri);
    void characters(const char *data, uint32_t length);

    bool isReadyWithStream() {
        return false;
    }

private:
    const OdfSaxAnalyzerFactory *m_factory;

    Strigi::AnalysisResult *m_result;
    const Strigi::RegisteredField *m_currentField;
};

#endif // ODFSAXANALYZER_H

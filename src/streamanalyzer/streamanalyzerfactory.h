/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *               2007 Tobias G. Pfeiffer <tgpfeiffer@web.de>
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
#ifndef STRIGI_STREAMANALYZERFACTORY
#define STRIGI_STREAMANALYZERFACTORY

#include <strigi/strigiconfig.h>

#include <vector>

namespace Strigi {

class FieldRegister;
class RegisteredField;

/**
 * The StreamAnalyzerFactory class
 */
class STREAMANALYZER_EXPORT StreamAnalyzerFactory {
private:
    class Private;
    Private* const p;
public:
    StreamAnalyzerFactory();
    virtual ~StreamAnalyzerFactory();
    /**
     * Returns the name of this analyzer factory. Taking the class name is fine
     * for this purpose.
     */
    virtual const char* name() const = 0;
    /**
     * Here you should register all fields that the corresponding
     * StreamLineAnalyzer is able to extract using FieldRegister::registerField().
     */
    virtual void registerFields(FieldRegister&) = 0;
    void addField(const RegisteredField*);
    const std::vector<const RegisteredField*>& registeredFields() const;
};

}

#endif

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
#ifndef ESTRAIERINDEXWRITER_H
#define ESTRAIERINDEXWRITER_H

#include "indexwriter.h"
#include <estraier.h>

class EstraierIndexManager;
class EstraierIndexWriter : public Strigi::IndexWriter {
friend class EstraierIndexManager;
private:
    EstraierIndexManager* manager;
    const std::string indexpath;

protected:
    void startAnalysis(Strigi::AnalysisResult*);
    void addField(const Strigi::AnalysisResult*, const Strigi::RegisteredField* field,
        const std::string& value);
    void addField(const Strigi::AnalysisResult*, const Strigi::RegisteredField* field,
        const unsigned char* data, uint32_t size) {}
    void addField(const Strigi::AnalysisResult*, const Strigi::RegisteredField* field,
        int32_t value) {}
    void addField(const Strigi::AnalysisResult*, const Strigi::RegisteredField* field,
        uint32_t value) {}
    void addField(const Strigi::AnalysisResult*, const Strigi::RegisteredField* field,
        double value) {}
    void addValue(const Strigi::AnalysisResult*, const Strigi::RegisteredField* field,
        const std::string& name, const std::string& value) {}
    void finishAnalysis(const Strigi::AnalysisResult*);
    void addText(const Strigi::AnalysisResult*, const char* text,
        int32_t length);
    void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) {}
    explicit EstraierIndexWriter(EstraierIndexManager*);
    ~EstraierIndexWriter();
public:
    void commit();
    void deleteEntries(const std::vector<std::string>& entries);
    void deleteAllEntries();
    int itemsInCache() { return 0; };
};

#endif

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
#ifndef CLUCENEINDEXWRITER_H
#define CLUCENEINDEXWRITER_H

#include <CLucene.h>
#include <strigi/strigiconfig.h>
#include <strigi/indexwriter.h>
#include <strigi/analysisresult.h>
#include <strigi/analyzerconfiguration.h>

class CLuceneIndexManager;
class CLuceneIndexWriter : public Strigi::IndexWriter {
private:
    CLuceneIndexManager* manager;
    int doccount;
    static const wchar_t* systemlocation();

protected:
    void startAnalysis(const Strigi::AnalysisResult*);
    void finishAnalysis(const Strigi::AnalysisResult*);
    void addText(const Strigi::AnalysisResult*, const char* text, int32_t length);
    static void addValue(const Strigi::AnalysisResult* idx,
        Strigi::AnalyzerConfiguration::FieldType type, const TCHAR* name,
        const TCHAR* value);
    static void addValue(const Strigi::AnalysisResult* idx,
        Strigi::AnalyzerConfiguration::FieldType type, const TCHAR* name,
        const std::string& value);
    void addValue(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, const std::string& value);
    void addValue(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* fieldname,
        const unsigned char* data, uint32_t size);
    void addValue(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* fieldname, uint32_t value);
    void addValue(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* fieldname, int32_t value);
    void addValue(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* fieldname, double value);
    void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) {}
    void addValue(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* field, const std::string& name,
        const std::string& value) {}
    void initWriterData(const Strigi::FieldRegister& f);
    void releaseWriterData(const Strigi::FieldRegister& f);
public:
    explicit CLuceneIndexWriter(CLuceneIndexManager* m);
    ~CLuceneIndexWriter();
    void commit();

    /** cleanup clucene files. this is run when clucene starts up */
    void cleanUp();
    void deleteEntries(const std::vector<std::string>& entries);
    void deleteAllEntries();
    void deleteEntry(const std::string& entry,
        lucene::index::IndexReader* reader);

    static const TCHAR* mapId(const TCHAR* id);
    static void addMapping(const TCHAR* from, const TCHAR* to);
};

#endif

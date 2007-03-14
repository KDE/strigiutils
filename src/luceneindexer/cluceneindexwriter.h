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

#include "cluceneindexer_export.h"
#include "indexwriter.h"
#include "analysisresult.h"
#include "analyzerconfiguration.h"

class CLuceneIndexManager;
class CLUCENEINDEXER_EXPORT CLuceneIndexWriter : public Strigi::IndexWriter {
private:
    CLuceneIndexManager* manager;
    int doccount;

protected:
    void startIndexable(Strigi::AnalysisResult*);
    void finishIndexable(const Strigi::AnalysisResult*);
    void addText(const Strigi::AnalysisResult*, const char* text, int32_t length);
    static void addField(const Strigi::AnalysisResult* idx,
        Strigi::AnalyzerConfiguration::FieldType type, const TCHAR* name,
        const TCHAR* value);
    static void addField(const Strigi::AnalysisResult* idx,
        Strigi::AnalyzerConfiguration::FieldType type, const TCHAR* name,
        const std::string& value);
    void addField(const Strigi::AnalysisResult* idx,
        const Strigi::RegisteredField* field, const std::string& value);
    void addField(const Strigi::AnalysisResult*,
        const Strigi::RegisteredField* fieldname,
        const unsigned char* data, int32_t size) {}
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
    void deleteEntry(const std::string& entry);

    static const TCHAR* mapId(const TCHAR* id);
    static void addMapping(const TCHAR* from, const TCHAR* to);
};

#endif

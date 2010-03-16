/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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

#ifndef STRIGI_LINEEVENTANALYZER
#define STRIGI_LINEEVENTANALYZER

#include <strigi/strigiconfig.h>
#include <strigi/streameventanalyzer.h>
#include <vector>
#include <string>
#include <iconv.h>

namespace Strigi {
class StreamLineAnalyzer;
class LineEventAnalyzer : public StreamEventAnalyzer {
private:
    std::vector<StreamLineAnalyzer*> line;
    bool* started;
    std::string byteBuffer;
    std::string ibyteBuffer;
    std::string lineBuffer;
    std::string encoding;
    AnalysisResult* result;
    iconv_t converter;
    const uint numAnalyzers;
    char* const convBuffer;
    unsigned char iMissingBytes;
    char missingBytes;
    bool ready;
    bool initialized;
    bool sawCarriageReturn;
    bool otherEncoding;

    const char* name() const { return "LineEventAnalyzer"; }
    void startAnalysis(AnalysisResult*);
    void endAnalysis(bool complete);
    void handleData(const char* data, uint32_t length);
    void handleUtf8Data(const char* data, uint32_t length);
    bool isReadyWithStream();
    void emitData(const char* data, uint32_t length);
    void initEncoding(std::string encoding);
public:
    LineEventAnalyzer(std::vector<StreamLineAnalyzer*>&s);
    ~LineEventAnalyzer();
};

}
#endif

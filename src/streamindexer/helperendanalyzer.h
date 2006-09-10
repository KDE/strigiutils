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
#ifndef HELPERENDANALYZER
#define HELPERENDANALYZER

#include "streamendanalyzer.h"
#include <vector>

class HelperProgramConfig {
public:
    class HelperRecord;
private:
    std::vector<HelperRecord*> helpers;

    std::string getPath(const std::string& exe,
        const std::vector<std::string>& paths) const;
public:
    HelperProgramConfig();
    ~HelperProgramConfig();
    HelperRecord* findHelper(const char* header, int32_t headersize) const;
};

class HelperEndAnalyzer : public jstreams::StreamEndAnalyzer {
private:
    static const HelperProgramConfig helperconfig;

    std::string writeToTempFile(jstreams::InputStream *in) const;
    bool checkForFile(int depth, const std::string& filename);
public:
    HelperEndAnalyzer() {}
    bool checkHeader(const char* header, int32_t headersize) const;
    char analyze(std::string filename, jstreams::InputStream *in, int depth,
        jstreams::StreamIndexer *indexer, jstreams::Indexable*);
    const char* getName() const { return "HelperEndAnalyzer"; }
};

#endif

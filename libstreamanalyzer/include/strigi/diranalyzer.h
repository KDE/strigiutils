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
#ifndef STRIGI_DIRANALYZER_H
#define STRIGI_DIRANALYZER_H

#include <strigi/strigiconfig.h>
#include <string>
#include <vector>

namespace Strigi {
class IndexManager;
class AnalyzerConfiguration;

class STREAMANALYZER_EXPORT AnalysisCaller {
public:
    virtual ~AnalysisCaller() {}
    virtual bool continueAnalysis() = 0;
};

class STREAMANALYZER_EXPORT DirAnalyzer {
public:
    class Private;
private:
    Private* const p;
public:
    DirAnalyzer(IndexManager& manager, AnalyzerConfiguration& conf);
    ~DirAnalyzer();
    int analyzeDir(const std::string& dir, int nthreads = 2,
        AnalysisCaller* caller = 0,
        const std::string& lastToSkip = std::string());
    int updateDir(const std::string& dir, int nthreads = 2,
        AnalysisCaller* caller = 0);
    int updateDirs(const std::vector<std::string>& dirs, int nthreads = 2,
        AnalysisCaller* caller = 0);
};
}
#endif

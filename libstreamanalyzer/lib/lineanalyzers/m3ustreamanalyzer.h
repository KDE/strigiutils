/* This file is part of the KDE project
 * Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
 * Copyright (C) 2007 Tim Beaulen <tbscope@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *  $Id$
 */

#ifndef M3USTREAMANALYZER_H
#define M3USTREAMANALYZER_H

#define STRIGI_IMPORT_API
#include <strigi/analyzerplugin.h>
#include <strigi/streamlineanalyzer.h>

class M3uLineAnalyzerFactory;

class M3uLineAnalyzer : public Strigi::StreamLineAnalyzer 
{
private:
    Strigi::AnalysisResult* analysisResult;
    const M3uLineAnalyzerFactory* factory;
    int32_t line;
    bool extensionOk;
    int32_t count;

public:
    M3uLineAnalyzer(const M3uLineAnalyzerFactory* f) : factory(f) {}
    ~M3uLineAnalyzer() {}

    const char* name() const { 
        return "M3uLineAnalyzer"; 
    }

    void startAnalysis(Strigi::AnalysisResult*);
    void handleLine(const char* data, uint32_t length);
    bool isReadyWithStream();
    void endAnalysis(bool complete);
};

class M3uLineAnalyzerFactory : public Strigi::StreamLineAnalyzerFactory 
{
friend class M3uLineAnalyzer;

private:
    const Strigi::RegisteredField* tracksField; //Number of tracks in the m3u playlist
    const Strigi::RegisteredField* trackPathField; //The paths to the tracks in the playlist
    const Strigi::RegisteredField* m3uTypeField; //The type of the m3u file, a simple list or an extended list

    const Strigi::RegisteredField* typeField;

    const char* name() const {
        return "M3uLineAnalyzer";
    }

    Strigi::StreamLineAnalyzer* newInstance() const {
        return new M3uLineAnalyzer(this);
    }

    void registerFields(Strigi::FieldRegister&);
};

class M3uFactoryFactory : public Strigi::AnalyzerFactoryFactory
{
public:
  std::list<Strigi::StreamLineAnalyzerFactory*> streamLineAnalyzerFactories() const {
     std::list<Strigi::StreamLineAnalyzerFactory*> af;
     af.push_back(new M3uLineAnalyzerFactory);
     return af;
  }
};

#endif

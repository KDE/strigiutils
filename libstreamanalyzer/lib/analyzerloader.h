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

#ifndef ANALYZERLOADER_H
#define ANALYZERLOADER_H

#include <strigi/strigiconfig.h>
#include <list>
#include <map>
#include <string>

namespace Strigi {

class StreamEndAnalyzerFactory;
class StreamSaxAnalyzerFactory;
class StreamLineAnalyzerFactory;
class StreamThroughAnalyzerFactory;
class StreamEventAnalyzerFactory;

class AnalyzerLoader {
    class Private;
public:
    static void loadPlugins(const char* dir);

    std::list<StreamEndAnalyzerFactory*> streamEndAnalyzerFactories();
    std::list<StreamThroughAnalyzerFactory*>
        streamThroughAnalyzerFactories();
    std::list<StreamSaxAnalyzerFactory*> streamSaxAnalyzerFactories();
    std::list<StreamLineAnalyzerFactory*> streamLineAnalyzerFactories();
    std::list<StreamEventAnalyzerFactory*> streamEventAnalyzerFactories();
};
}

#endif

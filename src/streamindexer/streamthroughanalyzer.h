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
#ifndef STREAMTHROUGHANALYZER_H
#define STREAMTHROUGHANALYZER_H

#include "inputstream.h"
namespace jstreams {
class Indexable;
/**
 * This class defines an interface for analyzing streams.
 * This interface is not yet complete, it lacks good functions for retrieving
 * the results in other ways than just printing them.
 */
class StreamThroughAnalyzer {
public:
    virtual ~StreamThroughAnalyzer() {};
    virtual void setIndexable(jstreams::Indexable*) = 0;
    /**
     * This function sets up the analyzer for handling a stream.
     * The stream \p in is used in constructing a new internal
     * inputstream that is returned by this function. Every time
     * a read call is performed on this class, the data read
     * is used in the analysis before being passed on to the
     * caller.
     **/
    virtual jstreams::InputStream *connectInputStream(jstreams::InputStream *in) = 0;
    /**
     *
     **/
    virtual bool isReadyWithStream() = 0;
};
}

#endif

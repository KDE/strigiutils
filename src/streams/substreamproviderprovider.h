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
#ifndef STRIGI_SUBSTREAMPROVIDERPROVIDER_H
#define STRIGI_SUBSTREAMPROVIDERPROVIDER_H

#include "streambase.h"

namespace Strigi {

class SubStreamProvider;

/**
 * @brief Creates a SubStreamProvider to extract substreams
 * from a given input stream.
 */
class SubStreamProviderProvider {
public:
    /** Initializes the SubStreamProviderProvider */
    SubStreamProviderProvider();
    /**
     * @brief Get a SubStreamProvider for an InputStream
     *
     * Analyzes the first few bytes of @p input in an attempt to
     * find a matching SubStreamProvider.  If it does so, it will
     * create a SubStreamProvider for @p input.
     *
     * @return a SubStreamProvider for @p input if a matching one
     * was found, 0 if no matching SubStreamProvider was found
     */
    SubStreamProvider* subStreamProvider(InputStream* input);
};

} // end namespace Strigi

#endif

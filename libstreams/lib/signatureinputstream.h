/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2009 Jos van den Oever <jos@vandenoever.info>
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
#ifndef STRIGI_SIGNATUREINPUTSTREAM
#define STRIGI_SIGNATUREINPUTSTREAM

#include <strigi/strigiconfig.h>
#include "streambase.h"

namespace Strigi {

/**
 * @brief Reads a stream and remembers the signature.
 *
 * This class stops reading data when either the end of the
 * underlying stream is reacher, or a given terminator is encountered
 * in the the stream.
 */
class SignatureInputStream : public InputStream {
private:
    class Private;
    Private* const p;
public:
    /**
     * @brief Create a stream from an InputStream.
     *
     * @param i the underlying InputStream to read the data from
     * @param terminator the terminator indicating the end of this substream
     */
    SignatureInputStream(InputStream* i, int32_t signaturesize);
    ~SignatureInputStream();
    /**
     * @brief inherited from StreamBase
     **/
    int32_t read(const char*& start, int32_t min=0, int32_t max=0);
    /**
     * @brief inherited from StreamBase
     **/
    int64_t reset(int64_t pos);
    /**
     * @brief inherited from StreamBase
     **/
    int64_t skip(int64_t ntoskip);
    /**
     * @brief Return the signature of this stream.
     * The signature is only available once the end of the stream has been
     * reached. The signature can up up to @p signaturesize bytes long.
     **/
    std::string signature() const;
};

} //end namespace Strigi

#endif

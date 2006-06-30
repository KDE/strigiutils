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
#ifndef DIGESTINPUTSTREAM_H
#define DIGESTINPUTSTREAM_H
#include "streambase.h"
#include <openssl/sha.h>

namespace jstreams {
class DigestInputStream : public StreamBase<char> {
private:
    bool finished;
    int32_t ignoreBytes;
    SHA_CTX sha1;
    unsigned char digest[SHA_DIGEST_LENGTH];
    StreamBase<char> *input;
#ifndef NDEBUG
    int64_t totalread;
#endif

    void finishDigest();
public:
    DigestInputStream(StreamBase<char> *input);
    int32_t read(const char*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t mark(int32_t readlimit);
    int64_t reset(int64_t);
    void printDigest();
    std::string getDigestString();
};
}
#endif

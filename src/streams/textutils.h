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
#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include <string>
#include "streams_export.h"

namespace jstreams {

STREAMS_EXPORT const char* checkUtf8(const char* p, int32_t length, char& nb);
 
STREAMS_EXPORT const char* checkUtf8(const std::string&, char& nb);

STREAMS_EXPORT bool checkUtf8(const char* p, int32_t length);

STREAMS_EXPORT bool checkUtf8(const std::string&);

STREAMS_EXPORT void convertNewLines(char* p);

#ifdef __BIG_ENDIAN__
STREAMS_EXPORT int16_t  readBigEndianInt16(const char* c) {
    return *reinterpret_cast<const int16_t*>(c);
}
STREAMS_EXPORT uint16_t readBigEndianUInt16(const char* c) {
    return *reinterpret_cast<const uint16_t*>(c);
}
STREAMS_EXPORT int32_t  readBigEndianInt32(const char* c) {
    return *reinterpret_cast<const int32_t*>(c);
}
STREAMS_EXPORT uint32_t readBigEndianUInt32(const char* c) {
    return *reinterpret_cast<const uint32_t*>(c);
}
STREAMS_EXPORT int64_t  readBigEndianInt64(const char* c) {
    return *reinterpret_cast<const int64_t*>(c);
}
STREAMS_EXPORT uint64_t readBigEndianUInt64(const char* c) {
    return *reinterpret_cast<const uint64_t*>(c);
}
STREAMS_EXPORT int16_t  readLittleEndianInt16(const char* c);
STREAMS_EXPORT uint16_t readLittleEndianUInt16(const char* c);
STREAMS_EXPORT int32_t  readLittleEndianInt32(const char* c);
STREAMS_EXPORT uint32_t readLittleEndianUInt32(const char* c);
STREAMS_EXPORT int64_t  readLittleEndianInt64(const char* c);
STREAMS_EXPORT uint64_t readLittleEndianUInt64(const char* c);
#else
STREAMS_EXPORT int16_t  readLittleEndianInt16(const char* c) {
    return *reinterpret_cast<const int16_t*>(c);
}
STREAMS_EXPORT uint16_t readLittleEndianUInt16(const char* c) {
    return *reinterpret_cast<const uint16_t*>(c);
}
STREAMS_EXPORT int32_t  readLittleEndianInt32(const char* c) {
    return *reinterpret_cast<const int32_t*>(c);
}
STREAMS_EXPORT uint32_t readLittleEndianUInt32(const char* c) {
    return *reinterpret_cast<const uint32_t*>(c);
}
STREAMS_EXPORT int64_t  readLittleEndianInt64(const char* c) {
    return *reinterpret_cast<const int64_t*>(c);
}
STREAMS_EXPORT uint64_t readLittleEndianUInt64(const char* c) {
    return *reinterpret_cast<const uint64_t*>(c);
}
STREAMS_EXPORT int16_t  readBigEndianInt16(const char* c);
STREAMS_EXPORT uint16_t readBigEndianUInt16(const char* c);
STREAMS_EXPORT int32_t  readBigEndianInt32(const char* c);
STREAMS_EXPORT uint32_t readBigEndianUInt32(const char* c);
STREAMS_EXPORT int64_t  readBigEndianInt64(const char* c);
STREAMS_EXPORT uint64_t readBigEndianUInt64(const char* c);
#endif


}

#endif

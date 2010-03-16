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
#ifndef STRIGI_STRINGSTREAM_H
#define STRIGI_STRINGSTREAM_H

/*
 * Author: Jos van den Oever
 *         Ben van Klinken
 **/

#include "streambase.h"

#include <string.h>

namespace Strigi {

/**
 * @brief Provides access to in-memory data thorugh the StreamBase API
 */
template <class T>
class StringStream : public StreamBase<T> {
private:
    int64_t markpt;
    T* data;
    bool dataowner;
    StringStream(const StringStream<T>&);
    void operator=(const StringStream<T>&);
public:
    /**
     * @brief Create a StringStream from a given string
     *
     * If the string you provide is not null-terminated, or
     * may not be null-terminated, you need to provide a length.
     * Note that if a length is provided, null characters have
     * no special meaning.
     *
     * If the string is created on the stack and may go out of
     * scope before the StringStream is destroyed, or if it is
     * created with @c new and may be destroyed before the
     * StringStream, you should set @p copy to @c true.
     *
     * @param value pointer to the data for this StringStream
     * @param length the length of the data at the pointer. If length is -1,
     * it is assumed @p value is null-terminated, and this is used to
     * find the end of the string
     * @param copy if true a copy of the string will be used, if false
     * a pointer to the original data will be used
     */
    StringStream(const T* value, int32_t length = -1, bool copy = true);
    ~StringStream();
    int32_t read(const T*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t reset(int64_t pos);
};

/** An InputStream to read from in-memory data. */
typedef StringStream<char> StringInputStream;

/** A Reader to read from in-memory data. */
typedef StringStream<wchar_t> StringReader;

template <class T>
StringStream<T>::StringStream(const T* value, int32_t length, bool copy)
        : markpt(0), dataowner(copy) {
    if (length < 0) {
        length = 0;
        while (value[length] != '\0') {
            length++;
        }
    }
    StreamBase<T>::m_size = length;
    if (copy) {
        data = new T[length+1];
        size_t s = (size_t)(length*sizeof(T));
        memcpy(data, value, s);
        data[length] = 0;
    } else {
        // casting away const is ok, because we don't write anyway
        data = (T*)value;
    }
}
template <class T>
StringStream<T>::~StringStream() {
    if (dataowner) {
        delete [] data;
    }
}
template <class T>
int32_t
StringStream<T>::read(const T*& start, int32_t min, int32_t max) {
    int64_t left = StreamBase<T>::m_size - StreamBase<T>::m_position;
    if (left == 0) {
        StreamBase<T>::m_status = Eof;
        return -1;
    }
    if (min < 0) min = 0;
    int32_t nread = (max > left || max < 1) ?(int32_t)left :max;
    start = data + StreamBase<T>::m_position;
    StreamBase<T>::m_position += nread;
    if (StreamBase<T>::m_position == StreamBase<T>::m_size) {
        StreamBase<T>::m_status = Eof;
    }
    return nread;
}
template <class T>
int64_t
StringStream<T>::skip(int64_t ntoskip) {
    if (ntoskip == 0) return 0;
    const T* start;
    return read(start, (int32_t)ntoskip, (int32_t)ntoskip);
}
template <class T>
int64_t
StringStream<T>::reset(int64_t newpos) {
    if (newpos < 0) {
        StreamBase<T>::m_status = Ok;
        StreamBase<T>::m_position = 0;
    } else if (newpos < StreamBase<T>::m_size) {
        StreamBase<T>::m_status = Ok;
        StreamBase<T>::m_position = newpos;
    } else {
        StreamBase<T>::m_position = StreamBase<T>::m_size;
        StreamBase<T>::m_status = Eof;
    }
    return StreamBase<T>::m_position;
}

} // end namespace Strigi

#endif

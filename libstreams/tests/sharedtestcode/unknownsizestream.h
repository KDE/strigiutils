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
#ifndef STRIGI_UNKNOWNSIZEINPUTSTREAM
#define STRIGI_UNKNOWNSIZEINPUTSTREAM

#include <strigi/strigiconfig.h>
#include <strigi/streambase.h>

namespace Strigi {

/**
 * @brief Stream for testing that has no known size until it reaches the end
 * This class is for internal use.
 */
class UnknownSizeInputStream : public InputStream {
private:
    InputStream* const input;
public:
    /**
     * @brief Create a stream from an InputStream.
     */
    UnknownSizeInputStream(InputStream* i) :input(i) {
        m_size = -1;
    }
    /**
     * @brief inherited from StreamBase
     **/
    int32_t read(const char*& start, int32_t min=0, int32_t max=0) {
        int32_t r = input->read(start, min, max);
        m_status = input->status();
        m_position = input->position();
        if (m_size == -1 && m_status == Eof) m_size = input->size();
        return r;
    }
    /**
     * @brief inherited from StreamBase
     **/
    int64_t reset(int64_t pos) {
        int64_t r = input->reset(pos);
        m_status = input->status();
        m_position = input->position();
        if (m_size == -1 && m_status == Eof) m_size = input->size();
        return r;
    }
    /**
     * @brief inherited from StreamBase
     **/
    int64_t skip(int64_t ntoskip) {
        int64_t r = input->skip(ntoskip);
        m_status = input->status();
        m_position = input->position();
        if (m_size == -1 && m_status == Eof) m_size = input->size();
        return r;
    }
};

} //end namespace Strigi

#endif

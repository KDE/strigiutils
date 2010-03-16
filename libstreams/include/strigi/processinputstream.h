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
#ifndef STRIGI_PROCESSINPUTSTREAM_H
#define STRIGI_PROCESSINPUTSTREAM_H

#include <strigi/streambase.h>
#include <strigi/bufferedstream.h>
#include <vector>

namespace Strigi {

class STREAMS_EXPORT ProcessInputStream : public BufferedInputStream {
private:
    InputStream *input;
    const char *const *args;
    int pid;
    int fdin;
    int fdout;

    void runCmd();
    void runCmdWithInput();
    void writeToPipe();
protected:
    int32_t fillBuffer(char* start, int32_t space);
public:
    explicit ProcessInputStream(const std::vector<std::string>& args,
        InputStream* input=0);
    ~ProcessInputStream();
};

} // end namespace Strigi

#endif

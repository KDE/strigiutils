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
#include "fsfileinputstream.h"
#include <QtCore/QFSFileEngine>

using namespace Strigi;

const int32_t FsFileInputStream::defaultBufferSize = 64;
FsFileInputStream::FsFileInputStream(const QString &filename, int32_t buffersize) {
    // initialize values that signal state
    m_status = Ok;
    fse = new QFSFileEngine(filename);

    // try to open the file for reading
    open = fse->open(QIODevice::ReadOnly);
    if (!open) {
        // handle error
        m_error = (const char*)fse->errorString().toUtf8();
        m_status = Error;
        return;
    }

    // allocate memory in the buffer
    //mark(buffersize);
}
FsFileInputStream::FsFileInputStream(QFSFileEngine *fse, int32_t buffersize) {
    m_status = Ok;
    open = true; // fse must be have been opened
    this->fse = fse;
    // allocate memory in the buffer
    //mark(buffersize);
}
FsFileInputStream::~FsFileInputStream() {
    delete fse;
}
StreamStatus
FsFileInputStream::reopen() {
    if (open) {
        if (!fse->seek(0)) {
            m_error = (const char*)fse->errorString().toUtf8();
            m_status = Error;
        }
    } else if (fse->open(QIODevice::ReadOnly)) {
        open = true;
    } else {
        m_error = (const char*)fse->errorString().toUtf8();
        m_status = Error;
    }
    resetBuffer();
    return m_status;
}
int32_t
FsFileInputStream::fillBuffer(char* start, int32_t space) {
    // prepare the buffer for writing
    // read into the buffer
    int32_t nwritten = (int32_t)fse->read(start, space);
    // check the file stream status
    if (nwritten == (int32_t)-1) {
        m_error = (const char*)fse->errorString().toUtf8();
        fse->close();
        open = false;
        m_status = Error;
    } else if (nwritten == 0) {
        fse->close();
        open = false;
        return -1;
    }
    return nwritten;
}


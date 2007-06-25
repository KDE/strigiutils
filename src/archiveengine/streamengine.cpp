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
#include "streamengine.h"
#include <QtCore/QDebug>
using namespace Strigi;

StreamEngine::StreamEngine(const FileEntry* e, ArchiveEngineBase* engine)
    : entry(e), archive(engine) {
    stream = 0;
}
StreamEngine::~StreamEngine() {
    delete archive;
}
QString
StreamEngine::fileName(FileName file) const {
    switch(file) {
    case PathName:
        return archive->fileName(DefaultName);
    case BaseName:
        return entry->name;
    case DefaultName:
    default:
        return archive->fileName(DefaultName)+'/'+entry->name;
    }
}
InputStream *
StreamEngine::inputStream() {
    if (stream == 0) {
        stream = archive->inputStream(entry);
    }
    return stream;
}
bool
StreamEngine::open(QIODevice::OpenMode mode) {
    if (mode != QIODevice::ReadOnly) {
        return 0;
    }
    if (stream == 0) {
        stream = archive->inputStream(entry);
    }
    return stream;
}
qint64
StreamEngine::read(char* data, qint64 maxlen) {
    if (maxlen == 0) {
        qDebug("maxlen == 0!!!\n");
        return 0;
    }
    if (stream) {
        const char *start;
        int32_t nread;
        if (maxlen > INT32MAX) maxlen = INT32MAX;
        nread = stream->read(start, (int32_t)maxlen, (int32_t)maxlen);
        if (nread > 0) {
            memcpy(data, start, nread);
            return nread;
        }
        if (nread == 0) {
            return 0;
        }
    } else {
        qDebug() << "no stream\n";
    }
    return -1;
}
/*qint64
StreamEngine::readLine(char *data, qint64 maxlen) {
    printf("readLine\n");
}*/
qint64
StreamEngine::size () const {
    return entry->size;
}

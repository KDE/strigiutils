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
#include <QtCore/QAbstractFileEngineHandler>
#include <QtCore/QDateTime>
#include "streambase.h"
#include "archiveengine.h"

class StreamEngine : public QAbstractFileEngine {
private:
    int32_t pos_;
    Strigi::InputStream *stream;
    const FileEntry* entry;
    ArchiveEngineBase *archive;

public:
    StreamEngine(const FileEntry* entry, ArchiveEngineBase* engine);
    ~StreamEngine();
    Strigi::InputStream *inputStream();
    FileFlags fileFlags ( FileFlags type = FileInfoAll ) const {
        return type & (QAbstractFileEngine::FileType|QAbstractFileEngine::ExistsFlag);
    }
    QString fileName ( FileName file = DefaultName ) const;
    bool open(QIODevice::OpenMode mode);
    qint64 read(char* data, qint64 maxlen);
//    qint64 readLine(char *data, qint64 maxlen);
    qint64 size () const;
    QDateTime fileTime ( FileTime time ) const {
        if (time == ModificationTime) {
            return entry->mtime;
        } else if (time == AccessTime) {
            return entry->atime;
        }
        return entry->ctime;
    }
    const FileEntry* fileEntry() const {
        return entry;
    }
};

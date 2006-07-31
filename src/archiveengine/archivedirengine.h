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
#ifndef ARCHIVEDIRENGINE
#define ARCHIVEDIRENGINE

#include "archiveengine.h"

class ArchiveDirEngine : public ArchiveEngineBase {
private:
    ArchiveEngineBase* parent;
//    const FileEntry* const entry;
protected:
    bool nextEntry() const {
        return parent->nextEntry();
    }
    void readEntryNames() const {
        parent->readEntryNames();
    }
public:
    ArchiveDirEngine(ArchiveEngineBase* parent,
        const FileEntry* e);
    ~ArchiveDirEngine();
    StreamEngine *openEntry(const QString& filename);
    ArchiveDirEngine *openDir(const QString& filename);
    QString fileName ( FileName file = DefaultName ) const;
    QStringList entryList(QDir::Filters filters,
        const QStringList& filterNames) const;
    jstreams::StreamBase<char>* getInputStream(const FileEntry* entry);
    FileFlags fileFlags ( FileFlags type = FileInfoAll ) const {
        // signal that this is file _and_ a "directory"
        FileFlags flags =
            QAbstractFileEngine::DirectoryType;
        return flags & type;
    }
    QDateTime fileTime ( FileTime time ) const {
        if (time == ModificationTime) {
            return entry->mtime;
        } else if (time == AccessTime) {
            return entry->atime;
        }
        return entry->ctime;
    }
};

#endif

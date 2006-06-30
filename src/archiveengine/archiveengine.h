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
#ifndef ARCHIVEENGINE
#define ARCHIVEENGINE

#include <QtCore/QAbstractFileEngineHandler>
#include <QtCore/QDateTime>
#include <QtCore/QHash>

class FileEntry;
class FileEntry {
private:
    QHash<const QString, FileEntry*> entries;
public:
    QString name;
    QDateTime mtime;
    QDateTime ctime;
    QDateTime atime;
    qint64 size;
    QAbstractFileEngine::FileFlags fileFlags;

    FileEntry(const QString &n):name(n){}
    ~FileEntry();
    FileEntry* add(const QString &name);
    FileEntry* getEntry(const QString &name);
    const FileEntry* getEntry(const QString &name) const;
    const QList<FileEntry*> getEntries() const {
        return entries.values();
    }
};

class StreamEngine;
class ArchiveDirEngine;

#include "fsfileinputstream.h"
#include "substreamprovider.h"

class ArchiveEngineBase : public QAbstractFileEngine {
friend class ArchiveDirEngine;
private:
    virtual bool nextEntry() const= 0;
    virtual void readEntryNames() const = 0;
protected:
    const FileEntry* entry;
public:
    virtual StreamEngine *openEntry(const QString &filename) = 0;
    virtual ArchiveDirEngine *openDir(QString filename) = 0;
    virtual jstreams::StreamBase<char>* getInputStream(const FileEntry* entry) = 0;
    // lose references to engines so that they are not deleted
    virtual void releaseEngines() {};
    bool caseSensitive () const { return true; }
    bool close () { return true; }
    bool copy ( const QString & /*newName*/ ) { return false; }
    bool isRelativePath () const {
        return false;
    }
    bool isSequential () const {
        return true;
    }
    bool open ( QIODevice::OpenMode /*mode*/ ) {
        printf("open archive");
        return false;
    }
    qint64 size() const { return entry->size; }
    QDateTime fileTime ( FileTime time ) const {
        if (time == ModificationTime) {
            return entry->mtime;
        } else if (time == AccessTime) {
            return entry->atime;
        }
        return entry->ctime;
    }
};

class FileEntryCache {
private:
    struct Entry {
        QDateTime lastUsed;
        FileEntry* entry;
    };
    static QHash<QString, Entry> entrycache;
    static int instances;
    void prune();
public:
    FileEntryCache();
    ~FileEntryCache();
    void addEntry(const QString& key, FileEntry*e);
    FileEntry* getEntry(const QString& key, const QDateTime &mtime);
};
/**
 * @short Implementation of QAbstractFileEngine that can open archives as directories.
 */
class ArchiveEngine : public ArchiveEngineBase {
private:
    QString fullpath;
    QString path;
    StreamEngine *streamengine;
    jstreams::StreamBase<char>* parentstream;
    jstreams::FSFileInputStream* filestream;
    QList<jstreams::StreamBase<char>*> compressedstreams;
    jstreams::SubStreamProvider *zipstream;
    mutable jstreams::StreamBase<char>* entrystream;
    mutable bool readAllEntryNames;
    FileEntry* rootentry;
    mutable FileEntry* current;
    static FileEntryCache cache;

    bool nextEntry() const;
    void openArchive();
    void readEntryNames() const;
    jstreams::StreamBase<char>* decompress(jstreams::StreamBase<char>*,
        int32_t bufsize) const;
    bool testStream(jstreams::StreamBase<char>* is, int32_t readsize) const;
    void getRootEntry(const QDateTime& mtime);
protected:
    const QLinkedList<FileEntry>* getEntries(const QString& base);
public:
    ArchiveEngine(const QString& path, QFSFileEngine *fs);
    ArchiveEngine(StreamEngine *fs);
    ~ArchiveEngine();
    StreamEngine *openEntry(const QString &filename);
    ArchiveDirEngine *openDir(QString filename);
    QStringList entryList(QDir::Filters filters,
        const QStringList& filterNames) const;
    QString fileName ( FileName file = DefaultName ) const;
    // lose references to engines so that they are not deleted
    void releaseEngines() {    streamengine = 0;};
    jstreams::StreamBase<char>* getInputStream(const FileEntry* entry);
    FileFlags fileFlags ( FileFlags type = FileInfoAll ) const {
        // signal that this is file _and_ a "directory"
        FileFlags flags =
            QAbstractFileEngine::DirectoryType|QAbstractFileEngine::FileType
            |QAbstractFileEngine::ExistsFlag;
        return flags & type;
    }
};

#endif

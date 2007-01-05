/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Ben van Klinken <bvklinken@gmail.com>
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
#include "archiveenginehandler.h"
#include "fsfileinputstream.h"
#include "archivereader.h"
#include <QtCore/QAbstractFileEngine>
#include <QtCore/QFSFileEngine>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
using namespace jstreams;
#include <iostream>
using namespace std;
#include <dirent.h>

class QFileStreamOpener : public StreamOpener {
public:
    ~QFileStreamOpener() {}
    StreamBase<char>* openStream(const string& url);
    int stat(const string& url, EntryInfo& e);
};
StreamBase<char>*
QFileStreamOpener::openStream(const string& url) {
    StreamBase<char>* stream = new FSFileInputStream(QString(url.c_str()));
    if (stream->getStatus() != Ok) {
        delete stream;
        stream = 0;
    }
    return stream;
}
int
QFileStreamOpener::stat(const string& url, EntryInfo& e) {
    QFSFileEngine f(url.c_str());
    QAbstractFileEngine::FileFlags flags = f.fileFlags(
        QAbstractFileEngine::ExistsFlag|
        QAbstractFileEngine::FileType|QAbstractFileEngine::DirectoryType);
    if (!(QAbstractFileEngine::ExistsFlag&flags)) {
        return -1;
    }
    e.type = EntryInfo::Unknown;
    if (flags & QAbstractFileEngine::FileType) {
        e.type = EntryInfo::File;
    }
    if (flags & QAbstractFileEngine::DirectoryType) {
        e.type = EntryInfo::Dir;
    }
    e.size = f.size();
    e.mtime = f.fileTime(QAbstractFileEngine::ModificationTime).toTime_t();
    QByteArray filename = f.fileName(QAbstractFileEngine::BaseName).toUtf8();
    e.filename.assign((const char*)filename, filename.length());
//    printf("name:'%s'\n", e.filename.c_str());

    return 0;
}

class StreamFileEngine : public QAbstractFileEngine {
private:
    const string url;
    ArchiveReader* reader;
    StreamBase<char>* stream;
    EntryInfo entryinfo;

public:
    StreamFileEngine(ArchiveReader* r, const string& u) :url(u) {
        reader = r;
        stream = 0;
        reader->stat(u, entryinfo);
    }
    ~StreamFileEngine();
    bool isSequential () const {
        return true;
    }
    bool open(QIODevice::OpenMode mode) {
        if (mode & QIODevice::WriteOnly && !(mode & QIODevice::ReadOnly)) {
            return false;
        }
        stream = reader->openStream(url);
//        printf("open %s\n", url.c_str());
        return stream;
    }
    bool close () {
        if (stream) {
            reader->closeStream(stream);
            stream=0;
        }
        return true;
    }
    bool isRelativePath () const {
        return false;
    }
    bool caseSensitive () const { return true; }
    qint64 size() const { return entryinfo.size; }
    QDateTime fileTime ( FileTime time ) const {
        QDateTime d;
        d.setTime_t(entryinfo.mtime);
        return d;
    }
    QStringList entryList(QDir::Filters filters,
        const QStringList & filterNames) const {
        QStringList l;
        DirLister dl = reader->getDirEntries(url);
        EntryInfo e;
        while (dl.nextEntry(e)) {
            l << e.filename.c_str();
        }
        return l;
    }
    QString fileName ( FileName file) const {
        switch(file) {
        case PathName:
            return url.c_str();
        case BaseName:
            return entryinfo.filename.c_str();
        case DefaultName:
        default:
            return url.c_str();
        }
        return QString(entryinfo.filename.c_str());
    }
    FileFlags fileFlags(FileFlags type) const;
    qint64 read(char* data, qint64 maxlen) {
        int nread = -1;
        if (stream && stream->getStatus() == Ok) {
            if (maxlen > 0) {
                const char* c;
                nread = stream->read(c, maxlen, maxlen);
                if (nread > 0) {
                    memcpy(data, c, nread);
                } else if (nread < 0) {
                    nread = -1;
                }
            } else {
                nread = 0;
            }
        }
//        cout << "read " << url << " maxlen " << maxlen << " " << nread << endl;
        return nread;
    }
};
StreamFileEngine::~StreamFileEngine() {
    close();
}
QAbstractFileEngine::FileFlags
StreamFileEngine::fileFlags(FileFlags type) const {
    FileFlags f = 0;
    if (entryinfo.type & EntryInfo::Dir) {
        f |= DirectoryType;
    }
    if (entryinfo.type & EntryInfo::File) {
        f |= FileType;
    }
    return f;
}
ArchiveEngineHandler::ArchiveEngineHandler() {
    reader = new ArchiveReader();
    opener = new QFileStreamOpener();
    reader->addStreamOpener(opener);
}
ArchiveEngineHandler::~ArchiveEngineHandler() {
    delete reader;
    delete opener;
}

QAbstractFileEngine *
ArchiveEngineHandler::create(const QString &fileName) const {
    string name((const char*)fileName.toUtf8());
    if (reader->canHandle(name)) {
        return new StreamFileEngine(reader, name);
    }
    return 0;
}

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
#include "archivedirengine.h"
#include "streamengine.h"
#include <QDebug>
using namespace jstreams;

ArchiveDirEngine::ArchiveDirEngine(ArchiveEngineBase* p, const FileEntry*e)
    : parent(p) {
    entry = e;
}
ArchiveDirEngine::~ArchiveDirEngine() {
    delete parent;
}
void
testSE(StreamEngine*e, const QString& f) {
    qDebug() << e << " " << f;
    char data[100];
        e->open(QIODevice::ReadOnly);
        int64_t n = e->read(data, 99);
        if (n == -1) {
        qDebug() << "could not read\n";
        } else {
        data[n] = '\0';
        qDebug() << data;
    }
}
StreamEngine*
ArchiveDirEngine::openEntry(const QString &filename) {
    const FileEntry *fe;
    int n=0;
    do {
        fe = entry->getEntry(filename);
        if (fe) {
            if (fe->fileFlags & QAbstractFileEngine::FileType) {
                StreamEngine* se = new StreamEngine(fe, this);
                testSE(se, filename);
                return se;
            }
            qDebug() << "0 " << filename;
            return 0;
        }
    } while (nextEntry());
    return 0;
}
ArchiveDirEngine*
ArchiveDirEngine::openDir(QString dirname) {
    const FileEntry *fe;
    do {
        fe = entry->getEntry(dirname);
        if (fe) {
            if (fe->fileFlags & QAbstractFileEngine::DirectoryType) {
                return new ArchiveDirEngine(this, fe);
            }
            return 0;
        }
    } while (nextEntry());
    return 0;
}
QString
ArchiveDirEngine::fileName(FileName file) const {
    QString name;
    switch(file) {
    case PathName:
        name = parent->fileName(DefaultName);
        break;
    case BaseName:
        name = entry->name;
        break;
    case DefaultName:
    default:
        name = parent->fileName(DefaultName)+'/'+entry->name;
    }
    return name;
}
QStringList
ArchiveDirEngine::entryList(QDir::Filters /*filters*/,
        const QStringList& /*filterNames*/) const {
    readEntryNames();
    // TODO: respect filters
    QStringList e;
    foreach (FileEntry *fe, entry->getEntries()) {
        e.append(fe->name);
    }
    return e;
}
InputStream *
ArchiveDirEngine::getInputStream(const FileEntry* entry) {
    return parent->getInputStream(entry);
}

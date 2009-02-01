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
#include "archiveenginehandler.h"
int
main(int argc, char** argv) {
    ArchiveEngineHandler engine;
    for (int i=1; i<argc; ++i) {
        QFileInfo info(argv[i]);
        if (info.isDir()) {
            QDir d(argv[i]);
            foreach(const QString& s, d.entryList()) {
                printf("%s\n", (const char*)s.toUtf8());
            }
        } else {
            QFile file(argv[i]);
            if (!file.open(QIODevice::ReadOnly)) {
                qDebug("Could not open '%s'\n", argv[i]);
                return 1;
            }
            const int bufsize=10;
            char buf[bufsize];
            qint64 read = file.read(buf, bufsize);
            while (read > 0) {
                fwrite(buf, read, 1, stdout);
                read = file.read(buf, bufsize);
            }
        }
    }
    return 0;
}

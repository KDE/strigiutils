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
#include "filebrowser.h"

#include <QtGui/QApplication>

int
main(int argc, char **argv) {
    QApplication app(argc, argv);

    ArchiveEngineHandler engine;

    if (argc > 1) {
        QFile file(argv[1]);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug("Could not open '%s'\n", argv[1]);
            return 1;
        }
        const int bufsize=10;
        char buf[bufsize];
        file.read(buf, bufsize);
        /*QStringList l;
        if (false) {
            l = engine.create(argv[1])->entryList(QDir::AllEntries, l);
        } else {
            QDir i(argv[1]);
            l = i.entryList();
        }
        printf("---\n");
        foreach(QString s, l) {
            printf("%s\n", (const char*)s.toUtf8());
        }
        printf("---\n"); */
        return 0;
    } else {
        FileBrowser fb;
        fb.show();
        return app.exec();
    }
}


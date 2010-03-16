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
#include "../mailinputstream.h"
#include "../fileinputstream.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

using namespace Strigi;

void
testMailFile2(StreamBase<char>* input) {
    MailInputStream mail(input);
    StreamBase<char>* a = mail.nextEntry();
    while (a) {
        std::string filename = "/tmp/mailtest/"+mail.entryInfo().filename;
        FILE* file = 0;
        if (mail.entryInfo().filename.length() > 0) {
            file = fopen(filename.c_str(), "w");
        }
        int32_t size = 0;
        bool readall = true;
        const char* start;
        int32_t nread = a->read(start, 1024, 0);
        //printf("# %i\n", nread);
        while (readall && nread > 0) {
            if (file) {
                //printf("writing: %i\t%i\t%s\n", nread, size, filename.c_str());
                fwrite(start, 1, nread, file);
            }
            size += nread;
            nread = a->read(start, 1, 0);
        }
        //if (size == 0) { a = 0; a->read(start, 1,1);}
        //printf(": %p\t%i\t%i\n", a, size, nread);
        if (file) {
            fclose(file);
        }
        a = mail.nextEntry();
    }
//    qDebug() << mail.subject().c_str();
//    qDebug() << mail.contentType().c_str();
}

void
testMailFile(const char* filename) {
    FileInputStream file(filename);
    file.mark(10000);
    const char* header;
    int32_t nread = file.read(header, 1024, 1024);
    bool ok = nread > 0 && MailInputStream::checkHeader(header, nread);
    if (ok) {
        file.reset(0);
        printf("mail %s\n", (const char*)filename);
        testMailFile2(&file);
    }
}

void
testMails(const char* path) {
    struct stat s;
    if (stat(path, &s) != 0) return;
    if (S_ISDIR(s.st_mode)) {
        std::string dir(path);
        if (dir[dir.length()-1] == '.') return;
        dir += '/';
        DIR* d = opendir(dir.c_str());
        if (d == 0) return;
        struct dirent* e = readdir(d);
        while(e) {
            std::string ep = dir+e->d_name;
            testMails(ep.c_str());
            e = readdir(d);
        }
        closedir(d);
    } else if (S_ISREG(s.st_mode)) {
        testMailFile(path);
    }
}

int
main(int argc, char **argv) {
    if (argc < 2) {
        std::string home(getenv("HOME"));
        std::string dir = home+"/.kde/share/apps/kmail/mail";
        testMails(dir.c_str());
        dir = home+"/Mail";
        testMails(dir.c_str());
    } else {
        for (int i=1; i<argc; ++i) {
            testMails(argv[i]);
        }
    }
    return 0;
}

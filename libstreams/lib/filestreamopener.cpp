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
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <strigi/strigiconfig.h>
#include <strigi/archivereader.h>
#include <strigi/fileinputstream.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace Strigi;

InputStream*
FileStreamOpener::openStream(const string& url) {
    InputStream* stream = new FileInputStream(url.c_str());
    if (stream->status() != Ok) {
        delete stream;
        stream = 0;
    }
    return stream;
}
int
FileStreamOpener::stat(const string& url, EntryInfo& e) {
    struct stat s;
    if (::stat(url.c_str(), &s) == -1) {
        return -1;
    }
    if (S_ISREG(s.st_mode)) {
        e.type = EntryInfo::File;
    } else if (S_ISDIR(s.st_mode)) {
        e.type = EntryInfo::Dir;
    } else {
        e.type = EntryInfo::Unknown;
    }
    e.size = s.st_size;
    e.mtime = s.st_mtime;
    size_t p = url.rfind('/');
    if (p == string::npos) {
        e.filename = url;
    } else {
        e.filename = url.substr(p+1);
    }

    return 0;
}

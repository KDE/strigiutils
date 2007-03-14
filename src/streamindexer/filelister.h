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
#ifndef FILELISTER_H
#define FILELISTER_H

/**
    This class does the same as the Posix command
     find $dir -type f -mmin -$age
    The performance is equal, but there are a couple of advantages:
      - the last modification date is given instead of the age of the file in
        minutes
      - the wanted file are returned as two strings for dirpath and filename
        through a callback function instead of one concatenated string
        through a pipe.
      - no need to spawn a separate process
**/

#include <sys/types.h>
#include <string>

#include "streamindexer_export.h"

#ifdef _WIN32
typedef unsigned int uid_t;
typedef unsigned int gid_t;
#endif

namespace jstreams {
    class AnalyzerConfiguration;
}

class STREAMINDEXER_EXPORT FileLister {
private:
    char* path;
    uint length;
    uid_t uid;
    gid_t gid;
    time_t m_oldestdate;
    void (*m_fileCallback)(const char* fullpath, uint dirlen, uint len,
        time_t mtime);
    void (*m_dirCallback)(const char* fullpath,  uint len);

    char* resize(uint len);
    void walk_directory(uint len);

    jstreams::AnalyzerConfiguration& m_config;
public:
    explicit FileLister(jstreams::AnalyzerConfiguration& ic);
    ~FileLister();
    /**
     * Specify the callback function that reports the files found.
         **/
    void setFileCallbackFunction(void (*callback)(const char* fullpath,
            uint dirlen, uint len, time_t mtime)) {
        m_fileCallback = callback;
    }

    void setDirCallbackFunction(
            void (*callback)(const char* fullpath, uint len)) {
        m_dirCallback = callback;
    }

    /**
     * List all the files in directory dir that have been modified more
     * recently then oldestdate.
     * dir should end with a /
     * on windows the root directory must be formatted like c:/ or c:\
     **/
    void listFiles(const char *dir, time_t oldestdate = 0);
};

/*!
* @param path string containing path to check
* Appends the terminating char to path.
* Under Windows that char is '\', '/' under *nix
*/
std::string fixPath (std::string path);

#endif

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

class FileLister {
private:
    char* path;
    uint length;
    time_t m_oldestdate;
    bool (*m_callback)(const char* fullpath, uint dirlen, uint len,
        time_t mtime);

    char* resize(uint len);
    bool walk_directory(uint len);
public:
    FileLister();
    ~FileLister();
    /**
     * Specify the callback function that reports the files found.
         **/
    void setCallbackFunction(bool (*callback)(const char* fullpath,
            uint dirlen, uint len, time_t mtime)) {
        m_callback = callback;
    }
    /**
     * List all the files in directory dir that have been modified more
     * recently then oldestdate.
     * dir should end with a /
     * on windows the root directory must be formatted like c:/ or c:\
     **/
    void listFiles(const char *dir, time_t oldestdate = 0);
};

#endif

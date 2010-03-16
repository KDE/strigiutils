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
#ifndef STRIGI_FILELISTER_H
#define STRIGI_FILELISTER_H
#include <sys/types.h>
#include <string>
#include <vector>
#include <sys/stat.h>

#include <strigi/strigiconfig.h>

namespace Strigi {
    class AnalyzerConfiguration;

}

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

namespace Strigi {

class FileLister {
private:
    class Private;
    Private* p;
public:
    FileLister(const Strigi::AnalyzerConfiguration* ic=0);
    ~FileLister();

    void startListing(const std::string& dir);
    /**
     * Thread-safe function for getting the next filename.
     @return length of the path assigned to the path variable or -1 if
             an error occurred
     */
    int nextFile(std::string& path, time_t& time);

    /**
     Tread-unsafe version to loop through all files.
     @return length of the path assigned to the path variable or -1 if
             an error occurred
     **/
    int nextFile(const char*& path, time_t& time);

    void skipTillAfter(const std::string& lastToSkip);
};

class STRIGI_EXPORT DirLister {
private:
    class Private;
    Private* p;
public:
    DirLister(const Strigi::AnalyzerConfiguration* ic=0);
    ~DirLister();

    void startListing(const std::string& dir);
    void stopListing();

    /**
     * Thread-safe function for getting the next filename.
     * @return 0 when no error occurred or -1 if an error occurred
     */
    int nextDir(std::string& path,
        std::vector<std::pair<std::string, struct stat> >& dirs);

    void skipTillAfter(const std::string& lastToSkip);
};
}
#endif

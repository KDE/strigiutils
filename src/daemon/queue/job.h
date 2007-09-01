/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#ifndef JOB_H
#define JOB_H

#include <time.h>

class Job {
private:
    time_t ctime;
    int priority;
public:
    Job(int p=0) :ctime(time(0)), priority(p) {}
    virtual ~Job() {}
    /*
     * do the job
     */
    virtual void run() = 0;
    virtual void stop() {}
    /*
     * Merge job @p j into this job and take ownership of @p j.
     * @return true if the merge was successful, false otherwise, in which case
     * the ownership is not transferred.
     */
    virtual bool merge(Job* j) { return false; }
    /*
     * Return true if this job has a lower priority or a later creation time
     */
    bool operator<(const Job& j) const {
        return priority < j.priority ||
            (priority == j.priority && ctime > j.ctime);
    }
};

#endif

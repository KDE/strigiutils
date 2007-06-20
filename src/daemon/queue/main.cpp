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
#include "job.h"
#include "jobqueue.h"
#include <iostream>
using namespace std;

class SleepJob : public Job {
private:
    int i, n;
public:
    SleepJob(int nn) :Job(nn), n(nn) {i=100000000;}
    ~SleepJob() {}
    void run() {}
    void stop() {i=0;}
};

int
main(int argc, char** argv) {
    JobQueue queue(10);
    for (int i=0; i<200000; ++i) {
        queue.addJob(new SleepJob(i));
    }
    return 0;
}

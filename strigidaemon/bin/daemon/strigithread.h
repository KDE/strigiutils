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
#ifndef STRIGITHREAD_H
#define STRIGITHREAD_H

#include <stdio.h>
#include <strigi/strigi_thread.h>
#include <string>

extern "C" void *threadstarter(void *);

class StrigiThread {
friend void* threadstarter(void *);
public:
    enum State {Idling, Working, Stopping};
private:
    State state;
protected:
    int priority;
    STRIGI_MUTEX_DEFINE(lock); 
    STRIGI_THREAD_DEFINE(thread);
    void setState(State s);
    virtual void* run(void*) = 0;
    virtual void stopThread() {}
public:
    const char* const name;

    explicit StrigiThread(const char* name);
    virtual ~StrigiThread();
    int start(int priority=0);
    void stop();
    void terminate();
    State getState();
    std::string getStringState();
    int getPriority() const { return priority; }
    static void stopThreads();
};

#endif


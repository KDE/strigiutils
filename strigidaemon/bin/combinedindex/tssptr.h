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

#ifndef TSSPTR_H
#define TSSPTR_H

#include "strigi/strigi_thread.h"
#include <strigi/indexpluginloader.h>

// thread safe smart pointer
template <class T>
class TSSPtr {
private:
    struct P {
        StrigiMutex lock;
        T* p;
        int count;
    };
    P* p;
public:
    TSSPtr(T* ptr = 0) {
        if (ptr) {
            p = new P();
            p->p = ptr;
            p->count = 1;
        } else {
            p = 0;
        }
    }
    TSSPtr(const TSSPtr& t) {
        set(t);
    }
    ~TSSPtr() {
        deref();
    }
    void operator=(const TSSPtr& t) {
        if (t.p == p) return;
        deref();
        set(t);
    }
    T* operator->() const {
        return (p) ?p->p :0;
    }
private:
    void deref() {
        if (p) {
            p->lock.lock();
            int c = --(p->count);
            p->lock.unlock();
            if (c == 0) {
                Strigi::IndexPluginLoader::deleteIndexManager(p->p);
                delete p;
            }
            p = 0;
        }
    }
    void set(const TSSPtr& t) {
        if (t.p) {
            t.p->lock.lock();
            t.p->count++;
            t.p->lock.unlock();
            p = t.p;
        } else {
            p = 0;
        }
    }
};

#endif

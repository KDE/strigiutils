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

#ifndef XESAMCLASS_H
#define XESAMCLASS_H

#include "strigi/strigi_thread.h"
#include <iostream>

class XesamClass {
private:
    STRIGI_MUTEX_DEFINE(mutex);
    int n;

    XesamClass(const XesamClass& x);
    void operator=(const XesamClass& );
public:
    XesamClass() :n(1) {
        STRIGI_MUTEX_INIT(&mutex);
    }
    virtual ~XesamClass() {
//        std::cerr << "delete " << this << std::endl;
        STRIGI_MUTEX_UNLOCK(&mutex);
        STRIGI_MUTEX_DESTROY(&mutex);
    }
    void ref() {
        STRIGI_MUTEX_LOCK(&mutex);
        n++;
        STRIGI_MUTEX_UNLOCK(&mutex);
    }
    void unref() {
        int m;
        STRIGI_MUTEX_LOCK(&mutex);
        m = --n;
        if (m == 0) {
            delete this;
        } else {
            STRIGI_MUTEX_UNLOCK(&mutex);
        }
    }
};

#endif

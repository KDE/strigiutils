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
#ifndef STRIGI_INDEXMANAGER_H
#define STRIGI_INDEXMANAGER_H

namespace Strigi {
class IndexReader;
class IndexWriter;

class IndexManager;
void deleteIndexManager(Strigi::IndexManager* m);

/**
 * Abstract interface that manages access to the IndexReader and IndexWriter
 * instances provided by a particular index backend.
 *
 * The IndexManager handles all access to a particular index through the
 * IndexReader and IndexWriter objects contained within it.
 * The functions of IndexManager are threadsafe, but the functions provided
 * by the IndexReader and IndexWriter need not be. IndexReader and IndexWriter
 * objects may not be accessed from different threads. When access to an
 * index from a particular thread is required, the functions indexReader()
 * and indexWriter() provide instances of the respective classes that may
 * be used in the active thread.
 **/
class IndexManager {
friend void deleteIndexManager(Strigi::IndexManager* m);
public:
    virtual ~IndexManager() {}
    /**
     * Provide access to an IndexReader object that may be used in the active
     * thread.
     *
     * @return a pointer to an IndexReader that reads from the index that is
     *         managed by this instance of IndexManager
     **/
    virtual IndexReader* indexReader() = 0;
    /**
     * Provide access to an IndexWriter object that may be used in the active
     * thread.
     *
     * @return a pointer to an IndexWriter that reads from the index that is
     *         managed by this instance of IndexManager
     **/
    virtual IndexWriter* indexWriter() = 0;
};
}

#endif

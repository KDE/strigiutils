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

#include "cluceneindexmanager.h"
#include <strigi/strigiconfig.h>
#include <CLucene.h>
#include <CLucene/store/RAMDirectory.h>
#include "cluceneindexwriter.h"
#include "cluceneindexreader.h"
#include <strigi/indexplugin.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "timeofday.h"
#include "../../../lib/stgdirent.h"

using namespace std;

/* define and export the index factory */
REGISTER_STRIGI_INDEXMANAGER(CLuceneIndexManager)

using namespace lucene::index;
using lucene::analysis::standard::StandardAnalyzer;
using lucene::store::FSDirectory;

Strigi::IndexManager*
createCLuceneIndexManager(const char* path) {
    return new CLuceneIndexManager(path);
}

int CLuceneIndexManager::numberOfManagers = 0;

CLuceneIndexManager::CLuceneIndexManager(const std::string& path)
        {//: bitsets(this) {
    ++numberOfManagers;
    dbdir = path;
    reader = 0;
    writer = 0;
    indexwriter = 0;
    indexreader = 0;
    openWriters = 0;
    otime.tv_sec = 0;
    otime.tv_usec = 0;
    writer = new CLuceneIndexWriter(this);
    analyzer = new StandardAnalyzer();
    if (path == ":memory:") {
        directory = new lucene::store::RAMDirectory();
    } else {
        try {
            directory = FSDirectory::getDirectory(path.c_str());
	    ((FSDirectory*)directory)->setFileMode(strtol("600", NULL, 8)); //TODO:  make configurable... how???
        } catch (CLuceneError& err) {
            fprintf(stderr, "could not create FSDirectory: %s\n", err.what());
            directory = 0;
        }
    }
    openWriter();
}
CLuceneIndexManager::~CLuceneIndexManager() {
    // close the writer and analyzer
    closeWriter();
    closeReader(); //TODO: - problem with field name intern...
    // no reader or writer should be referring to this directory anymore
    if (directory){
      directory->close();
      delete directory;
    }
    delete writer;
    delete reader;
    delete analyzer;
    if (--numberOfManagers == 0) {
// temporarily commented out because of problem with clucene
//        _lucene_shutdown();
    }
}
Strigi::IndexReader*
CLuceneIndexManager::indexReader() {
    return luceneReader();
}
CLuceneIndexReader*
CLuceneIndexManager::luceneReader() {
    readlock.lock();
    if (reader == 0) {
      reader = new CLuceneIndexReader(this, dbdir);
    }
    readlock.unlock();
    return reader;
}
Strigi::IndexWriter*
CLuceneIndexManager::indexWriter() {
    return writer;
}
void
CLuceneIndexManager::openWriter(bool truncate) {
    if (directory == 0) return;
    try {
        bool create = truncate || !IndexReader::indexExists(directory);
        if (!create) {
          //TODO: this seems a bit dangerous!!!???
            //write custom LockFactory which puts the process id in it, and can search for the process?
	    if (IndexReader::isLocked(directory)) {
                IndexReader::unlock(directory);
            }
        }
printf("HI\n");
        indexwriter = new IndexWriter(directory, analyzer, create);
printf("HI2\n");
        indexwriter->setMaxFieldLength(10000); //TODO: make configurable... how???
        indexwriter->setInfoStream(&cerr); //TODO: make configurable - debug? how???
        indexwriter->setRAMBufferSizeMB(16.0); //TODO: make configurable... how???
        indexwriter->setTermIndexInterval(128); //TODO:  make configurable... how???
	
        //other interesting settings:
        //indexwriter->setUseCompoundFile(true); //TODO: do this when using memory - faster (but risks 'too many open files' error)
        //setMaxBufferedDeleteTerms - sets max number of deleted items in memory before flushing (by default is when ram gets full)
        //setMergePolicy
        //setMergeScheduler
        //setMaxMergeDocs - defaults to INT_MAX - uses RAM size instead...
        //setMergeFactor - default 10. >10 for batch, <10&&>2 for interactive
        //setMaxBufferedDocs



    } catch (CLuceneError& err) {
        fprintf(stderr, "could not create writer: %s\n", err.what());
        indexwriter = 0;
    }
}
void
CLuceneIndexManager::closeWriter() {
		writelock.lock();
    if (indexwriter == 0) {
				writelock.unlock();
        return;
    }

    //wait until all writers are finished...
    int c = 0;
    while ( openWriters > 0 && c < 5 ){
      writelock.unlock();
      printf("waiting for %d writers to close... (%d)\n", openWriters, c+1);
      sleep(1);
      writelock.lock();
      c++;
    }

    try {
        indexwriter->flush();
        //TODO: currently causes segfault... still investigating...
        indexwriter->close();
        delete indexwriter;
    } catch (CLuceneError& err) {
        printf("could not close writer: %s\n", err.what());
    }
    indexwriter = 0;
    // clear the cache
    //bitsets.clear();
    writelock.unlock();
}
IndexWriter*
CLuceneIndexManager::refWriter() {
    writelock.lock();
    if (indexwriter == 0) {
        openWriter();
    }
    openWriters++;
    writelock.unlock();
    return indexwriter;
}
void
CLuceneIndexManager::derefWriter() {
    writelock.lock();
    openWriters--;
    writelock.unlock();
}

void
CLuceneIndexManager::openReader() {
  readlock.lock();
  bool opened = false;
  if ( indexreader == NULL || !indexreader->isCurrent() ){
    opened = true;
    try {
printf("opening reader %s\n", dbdir.c_str());
        indexreader = lucene::index::IndexReader::open(directory);

        //other interesting settings:
        //setTermInfosIndexDivisor
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not create reader %s: %s\n", dbdir.c_str(),
            err.what());
        indexreader = 0;
    }
  }else if ( !indexreader->isCurrent() ){
    opened = true;
    try {
printf("re-opening reader %s\n", dbdir.c_str());
      lucene::index::IndexReader* newreader = indexreader->reopen();
      if (newreader != indexreader) {
        // reader was reopened
        indexreader->close();
        _CLDELETE(indexreader);
      }
      indexreader = newreader;
    } catch (CLuceneError& err) {
        fprintf(stderr, "could not reopen reader %s: %s\n", dbdir.c_str(),
            err.what());
        reader = 0;
    }
  }

  if ( opened ){
    gettimeofday(&otime, 0);
    doccount = -1;
    wordcount = -1;
  }
  readlock.unlock();
}
void
CLuceneIndexManager::closeReader() {
  readlock.lock();
  if (indexreader == 0) return;
  try {
      indexreader->close();
  } catch (CLuceneError& err) {
      fprintf(stderr, "could not close clucene: %s\n", err.what());
  }
  delete indexreader;
  indexreader = 0;
  readlock.unlock();
}
lucene::index::IndexReader*
CLuceneIndexManager::checkReader(bool enforceCurrent) {
    if ( indexreader != NULL && !indexreader->isCurrent() ) {
        if (enforceCurrent) {
            openReader();
        } else {
            struct timeval now;
            gettimeofday(&now, 0);
            if (now.tv_sec - otime.tv_sec > 60) {
                openReader();
            }
        }
    } else if (indexreader == 0) {
        openReader();
    }
    return indexreader;
}

int32_t
CLuceneIndexManager::countDocuments() {
    lucene::index::IndexReader* reader = checkReader(true);
    if (reader == NULL) return -1;
    if (doccount == -1) {
        doccount = reader->numDocs();
    }
    return doccount;
}
int32_t
CLuceneIndexManager::countWords() {
    lucene::index::IndexReader* reader = checkReader();
    if (reader == NULL) return -1;
    if (wordcount == -1) {
        if (reader) {
            wordcount = 0;
            lucene::index::TermEnum *terms = reader->terms();
            while (terms->next()) wordcount++;
            _CLDELETE(terms);
        }
    }
    return wordcount;
}
int64_t
CLuceneIndexManager::indexSize() {
    // sum the sizes of the files in the index
    // loop over directory entries
    DIR* dir = opendir(dbdir.c_str());
    if (dir == 0) {
        fprintf(stderr, "could not open index directory %s (%s)\n", dbdir.c_str(), strerror(errno));
        return -1;
    }
    struct dirent* e = readdir(dir);
    int64_t size = 0;
    while (e != 0) {
        string filename = dbdir+'/'+e->d_name;
        struct stat s;
        int r = stat(filename.c_str(), &s);
        if (r == 0) {
            if ( S_ISREG(s.st_mode)) {
                size += s.st_size;
            }
        } else {
            fprintf(stderr, "could not open file %s (%s)\n", filename.c_str(), strerror(errno));
        }
        e = readdir(dir);
    }
    closedir(dir);

    //TODO: can also add indexwriter->ramSizeInBytes()
    return size;
}


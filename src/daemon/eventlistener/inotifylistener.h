/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Flavio Castelli <flavio.castelli@gmail.com>
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
#ifndef INOTIFYLISTENER_H
#define INOTIFYLISTENER_H

#include "eventlistener.h"
#include "strigi_thread.h"
#include <map>
#include <vector>

class Event;
class PollingListener;

/*!
* @class InotifyListener
* @brief Interacts with kernel inotify monitoring recursively all changes over indexed directories
*/

class InotifyListener : public EventListener
{
    public:

        /*!
        * @class ReindexDirsThread
        * @brief Simple thread called when user changes indexed dirs
        *
        * It's a separate thread that takes care of updating inotify watches and indexed files according to the new directories specified by the user
.        */
        class ReindexDirsThread : public StrigiThread
        {
            friend class InotifyListener;

            public:
                explicit ReindexDirsThread (const char* name, const std::set<std::string> &newdirs, const std::set<std::string> &olddirs) : StrigiThread (name) {
                    m_pFilterManager = NULL;
                    m_pIndexReader = NULL;
                    m_newDirs = newdirs;
                    m_oldDirs = olddirs;
                    m_bfinished = false;
                    STRIGI_MUTEX_INIT (&m_nextJobLock);
                    STRIGI_MUTEX_INIT (&m_resourcesLock);
                }

                ~ReindexDirsThread();

                void* run(void*);

                void setFilterManager (FilterManager* filterManager) {
                    m_pFilterManager = filterManager;
                }

                void setIndexReader (jstreams::IndexReader* ireader) {
                    m_pIndexReader = ireader;
                }

                void setIndexedDirs (const std::set<std::string>& dirs);

            protected:
                void reindex();

                /*!
                * Method called by FileLister on every dir.
                * Used for adding files to db
                * @sa FileLister
                */
                static bool indexFileCallback(const char* path, uint dirlen, uint len, time_t mtime);

                /*!
                * Method called by FileLister on every dir.
                * Used for adding watches over dirs.
                * @sa FileLister
                */
                static void watchDirCallback(const char* path, uint len);

                FilterManager* m_pFilterManager;
                jstreams::IndexReader* m_pIndexReader;
                std::map<std::string, time_t> m_toIndex; //!< new files to index
                std::set<std::string> m_toWatch; //!< new directories to watch
                std::vector<Event*> m_events;
                std::set<std::string> m_newDirs; //!< new indexed dirs specified by the user
                std::set<std::string> m_oldDirs; //!< old indexed dirs
                std::set<std::string> m_nextJobDirs; //!< new dirs to index, user changed indexed dirs more than one time
                std::set<std::string> m_nomoreIndexedDirs; //!< dirs no more indexed
                bool m_bfinished; //!< true if thread has nothing left to do
                pthread_mutex_t m_nextJobLock;//!< mutex lock over m_nextJobDirs
                pthread_mutex_t m_resourcesLock; //!< mutex lock over all variables (excluding m_nextJobDirs)
                static ReindexDirsThread* workingReindex; //!<pointer to current ReindexDirsThread instance, used with FileLister's callbacks
        };

        explicit InotifyListener(std::set<std::string>& indexedDirs);

        ~InotifyListener();

        bool init();

        bool addWatch (const std::string& path);
        void addWatches (const std::set<std::string>& watches);
        void setIndexedDirectories (const std::set<std::string>& dirs);

        void* run(void*);

    protected:
        /*!
        * Method called by FileLister on every dir.
        * Used for adding files to db
        * @sa FileLister
        */
        static bool indexFileCallback(const char* path, uint dirlen, uint len, time_t mtime);

        /*!
        * Method called by FileLister on every dir.
        * Used for adding watches over dirs.
        * @sa FileLister
        */
        static void watchDirCallback(const char* path, uint len);

        /*!
        * @param dirs all dirs to watch over, to scan recursively
        * initialization procedure, used at startup
        */
        void bootstrap (const std::set<std::string>& dirs);

        /*!
        * @param event inotify's event mask
        * returns a string containing the event description
        */
        std::string eventToString(int events);

        /*!
        * @param event the inotify event to analyze
        * returns true if event is to process (ergo is interesting), false otherwise
        */
        bool isEventInteresting (struct inotify_event * event);

        /*!
        * main InotifyListener thread
        */
        void watch ();

        /*!
        * @param dir removed dir
        * Removes all db entries of files contained into the removed dir.
        * Removes also all inotify watches related to removed dir (including watches over subdirs), there's <b>no need</b> to call rmWatch after invoking that method
        * Updates also m_watches
        */
        void dirRemoved (std::string dir, std::vector<Event*>& events);

        /*!
        * @param dirs removed dirs
        * Removes all db entries of files contained into the removed dirs.
        * Removes also all inotify watches related to removed dirs (including watches over subdirs), there's <b>no need</b> to call rmWatch after invoking that method
        * Optimized for large removal
        * Updates also m_watches
        */
        void dirsRemoved (std::set<std::string> dirs, std::vector<Event*>& events);

        /*!
        * @param wd inotify watch descriptor
        * @param path path associated to inotify watch
        * removes and release an inotify watch. Usually there's no need to use this method.
        * @sa dirRemoved
        */
        void rmWatch(int wd, std::string path);

        /*!
        * removes and release all inotify watches
        */
        void clearWatches();

        PollingListener* m_pollingListener;
        int m_iInotifyFD;
        int m_iEvents;
        std::map<int, std::string> m_watches; //!< map containing all inotify watches added by InotifyListener. Key is watch descriptor, value is dir path
        bool m_bMonitor;
        bool m_bInitialized;
        std::map<std::string, time_t> m_toIndex;
        std::set<std::string> m_toWatch;
        std::set<std::string> m_indexedDirs;
        pthread_mutex_t m_watchesLock;
        ReindexDirsThread* m_reindexDirThread;
        static InotifyListener* workingInotifyListener; //!<pointer to current InotifyListener instance, used with FileLister's callbacks
};

#endif

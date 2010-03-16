/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Flavio Castelli <flavio.castelli@gmail.com>
 *                    Jos van den Oever <jos@vandenoever.info>
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

#include "daemonconfigurator.h"
#include "strigilogging.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using namespace std;
using namespace Strigi;

class FindRepository {
private:
    string m_repoName;

public:
    FindRepository(string repoName) { m_repoName = repoName; }

    bool operator()(Repository repo) {
        if (repo.a_name.compare(m_repoName) == 0) {
            return true;
        }
        return false;
    }
};

DaemonConfigurator::DaemonConfigurator (const string& confFile)
        : m_confFile (confFile) {
    // find the strigi directory
    string strigidir;
    string::size_type slashpos = confFile.rfind('/');
    if (slashpos != string::npos) {
        strigidir = confFile.substr(0, slashpos);
    } else {
        strigidir = getenv("HOME") + string("/.strigi");
    }

    stringbuf xml;
    ifstream f(confFile.c_str(), ios::binary);
    f.get(xml, '\0');
    f.close();

    if (xml.str().length()) {
        XMLStream stream(xml.str());
        stream >> *this;
    }
    FindRepository findRepository("localhost");
    list<Repository>::iterator match = find_if(e_repository.begin(),
                                               e_repository.end(),
                                               findRepository);

    // entry for localhost repository doesn't exists
    if (match == e_repository.end()) {
        a_useDBus = true;
        Repository r;
        r.a_name = "localhost";
        string home;
        if (getenv("HOME")) {
            home.assign(getenv("HOME"));
        }
        r.a_indexdir = strigidir + "/clucene";
        r.a_writeable = true;
        r.a_type = "clucene";
        r.a_pollingInterval = DEFAULT_POLLING_INTERVAL;

        Path p;
        p.a_path = home;                 r.e_path.push_back(p);
        p.a_path = home + "/.kde";       r.e_path.push_back(p);
        p.a_path = home + "/.kde4";      r.e_path.push_back(p);
        p.a_path = home + "/.gnome2";    r.e_path.push_back(p);
        p.a_path = home + "/.evolution"; r.e_path.push_back(p);
        p.a_path = home + "/.mozilla";   r.e_path.push_back(p);
        p.a_path = home + "/.mozilla-thunderbird";   r.e_path.push_back(p);
        e_repository.push_back(r);

        // add pattern to ignore hidden directories and hidden files
        Filter f;
        f.a_include = true;
        f.a_pattern = ".*.directory/"; // mail box directory
        e_filters.e_filter.push_back(f);
        f.a_include = false;
        f.a_pattern = ".*/";
        e_filters.e_filter.push_back(f);
        f.a_pattern = ".*";
        e_filters.e_filter.push_back(f);

        STRIGI_LOG_WARNING ("DaemonConfigurator",
                            "created default config for indexed dirs");
    }
    save();
}
void
DaemonConfigurator::setIndexedDirectories ( set<string>& dirs,
                                            const string& repositoryName,
                                            bool  merge) {
    FindRepository findRepository (repositoryName);

    list<Repository>::iterator match = find_if (e_repository.begin(),
                                                e_repository.end(),
                                                findRepository);

    Repository* r;

    if (match != e_repository.end()) {
        r = &(*match);
    } else {
        // create a new repository
        Repository repo;
        repo.a_name = repositoryName;
        e_repository.push_back(repo);
        return setIndexedDirectories (dirs, repositoryName, merge);
    }

    // clear old path
    if (!merge) {
        r->e_path.clear();
    }

    for (set<string>::const_iterator iter = dirs.begin(); iter != dirs.end();
            ++iter) {
        Path p;
        p.a_path = *iter;
        r->e_path.push_back(p);
    }
}
set<string>
DaemonConfigurator::getIndexedDirectories(const string& repositoryName) {
    set<string> dirs;

    FindRepository findRepository (repositoryName);

    list<Repository>::iterator match = find_if (e_repository.begin(),
                                                e_repository.end(),
                                                findRepository);

    if (match == e_repository.end()) {
        STRIGI_LOG_WARNING ("DaemonConfigurator.getIndexedDirs",
                       "cannot find repository name: |" + repositoryName + '|');
        return dirs;
    }

    Repository* r = &(*match);
    
    for (list<Path>::const_iterator iter = r->e_path.begin();
         iter != r->e_path.end(); ++iter) {
        dirs.insert (iter->a_path);
    }

    return dirs;
}
void
DaemonConfigurator::save(const char* file) {
    ofstream f;
    if (file == NULL) {
        f.open(m_confFile.c_str(), ios::binary);
    } else {
        f.open(file, ios::binary);
    }
    f << *this;
    f.close();
}
string
DaemonConfigurator::getWriteableIndexType() const {
    return (e_repository.size()) ?e_repository.begin()->a_type : "";
}
string
DaemonConfigurator::getWriteableIndexDir() const {
    return (e_repository.size()) ?e_repository.begin()->a_indexdir : "";
}
list<Repository>
DaemonConfigurator::getReadOnlyRepositories() const {
    list<Repository> ro;
    list<Repository>::const_iterator i;
    for (i = e_repository.begin(); i != e_repository.end(); ++i) {
        if (!i->a_writeable) {
            ro.push_back(*i);
        }
    }
    return ro;
}
void
DaemonConfigurator::setPollingInterval (unsigned int value,
                                        const string& repositoryName) {
    FindRepository findRepository (repositoryName);

    list<Repository>::iterator match = find_if (e_repository.begin(),
                                                e_repository.end(),
                                                findRepository);

    if (match != e_repository.end()) {
        Repository* r = &(*match);
        r->a_pollingInterval = value;
    } else {
        // create a new repository
        Repository repo;
        repo.a_name = repositoryName;
        repo.a_pollingInterval = value;
        e_repository.push_back(repo);
    }
}

unsigned int
DaemonConfigurator::getPollingInterval(const string& repositoryName) {
    FindRepository findRepository (repositoryName);

    list<Repository>::iterator match = find_if (e_repository.begin(),
                                                e_repository.end(),
                                                findRepository);

    Repository* r;

    if (match != e_repository.end()) {
        r = &(*match);
    } else {
        // create a new repository
        Repository repo;
        repo.a_name = repositoryName;
        e_repository.push_back(repo);
        r = &repo;
    }

    // check minimum polling interval
    if (r->a_pollingInterval < 5)
    {
        r->a_pollingInterval = DEFAULT_POLLING_INTERVAL;
        return DEFAULT_POLLING_INTERVAL;
    } else {
        return r->a_pollingInterval;
    }
}
void
DaemonConfigurator::loadFilteringRules(AnalyzerConfiguration& config) {
    vector<pair<bool,string> > filters;
    list<Filter>::const_iterator i;
    for (i = e_filters.e_filter.begin(); i != e_filters.e_filter.end(); ++i) {
        filters.push_back(make_pair<bool,string>(i->a_include, i->a_pattern));
    }
    config.setFilters(filters);
}
void
DaemonConfigurator::saveFilteringRules(const vector<pair<bool,string> >& f) {
    e_filters.e_filter.clear();
    vector<pair<bool,string> >::const_iterator i;
    for (i = f.begin(); i != f.end(); ++i) {
        Filter f;
        f.a_include = i->first;
        f.a_pattern = i->second;
        e_filters.e_filter.push_back(f);
    }
}

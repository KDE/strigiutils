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
#include "../strigilogging.h"

#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;
using namespace jstreams;

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
    : m_confFile (confFile)
{
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
        string s = getenv("HOME");
        r.a_indexdir = s + "/.strigi/clucene";
        r.a_writeable = true;
        r.a_type = "clucene";
        Path p;
        p.a_path = s;                 r.e_path.push_back(p);
        p.a_path = s + "/.kde";       r.e_path.push_back(p);
        p.a_path = s + "/.gnome2";    r.e_path.push_back(p);
        p.a_path = s + "/.evolution"; r.e_path.push_back(p);
        p.a_path = s + "/.mozilla";   r.e_path.push_back(p);
        p.a_path = s + "/.mozilla-thunderbird";   r.e_path.push_back(p);
        e_repository.push_back(r);

        // add pattern to ignore hidden directories

        STRIGI_LOG_WARNING ("DaemonConfigurator",
                            "created default config for indexed dirs")
    }

//pattern='*/.*'>
// </patternfilter>
// */.*
}
void
DaemonConfigurator::setIndexedDirectories ( set<string>& dirs,
                                            const string& repositoryName,
                                            bool  merge)
{
    FindRepository findRepository (repositoryName);

    list<Repository>::iterator match = find_if (e_repository.begin(),
                                                e_repository.end(),
                                                findRepository);

    Repository* r;

    if (match != e_repository.end())
        r = &(*match);
    else
    {
        // create a new repository
        Repository repo;
        repo.a_name = repositoryName;
        e_repository.push_back(repo);
        r = &repo;
    }

    // clear old path
    if (!merge)
        r->e_path.clear();

    for (set<string>::const_iterator iter = dirs.begin(); iter != dirs.end(); iter++)
    {
        Path p;
        p.a_path = *iter;
        r->e_path.push_back(p);
    }
}
set<string>
DaemonConfigurator::getIndexedDirectories (const string& repositoryName)
{
    set<string> dirs;

    FindRepository findRepository (repositoryName);

    list<Repository>::iterator match = find_if (e_repository.begin(),
                                                e_repository.end(),
                                                findRepository);

    if (match == e_repository.end())
    {
        STRIGI_LOG_WARNING ("DaemonConfigurator.getIndexedDirs",
                        "cannot find repository name: |" + repositoryName + "|")
        return dirs;
    }

    for (list<Path>::const_iterator iter = match->e_path.begin();
         iter != match->e_path.end(); iter++)
    {
        dirs.insert (iter->a_path);
    }

    return dirs;
}
void
DaemonConfigurator::save(const char* file) {
    ofstream f;
    if (file == NULL)
        f.open(m_confFile.c_str(), ios::binary);
    else
        f.open(file, ios::binary);
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
                                        const string& repositoryName)
{
    FindRepository findRepository (repositoryName);

    list<Repository>::iterator match = find_if (e_repository.begin(),
                                                e_repository.end(),
                                                findRepository);

    Repository* r;

    if (match != e_repository.end())
        r = &(*match);
    else
    {
        // create a new repository
        Repository repo;
        repo.a_name = repositoryName;
        repo.a_pollingInterval = 3; // default 3 minutes
        e_repository.push_back(repo);
        r = &repo;
    }

    r->a_pollingInterval = value;
}

unsigned int
DaemonConfigurator::getPollingInterval(const string& repositoryName)
{
    FindRepository findRepository (repositoryName);

    list<Repository>::iterator match = find_if (e_repository.begin(),
                                                e_repository.end(),
                                                findRepository);

    Repository* r;

    if (match != e_repository.end())
        r = &(*match);
    else
    {
        // create a new repository
        Repository repo;
        repo.a_name = repositoryName;
        e_repository.push_back(repo);
        r = &repo;
    }

    // minimum polling interval is 60 seconds
    if (r->a_pollingInterval > 0 && r->a_pollingInterval < 180)
        return 180;
    else
        return r->a_pollingInterval;
}
void
DaemonConfigurator::loadFilteringRules(IndexerConfiguration& config) {
    vector<pair<bool,string> > filters;
    list<Filter>::const_iterator i;
    for (i = e_filters.e_filter.begin(); i != e_filters.e_filter.end(); ++i) {
        filters.push_back(make_pair(i->a_include, i->a_pattern));
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

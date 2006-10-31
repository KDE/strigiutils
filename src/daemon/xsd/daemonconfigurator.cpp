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

#include "daemonconfigurator.h"
#include "../strigilogging.h"

#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

class FindRepository
{
    private:
        string m_repoName;

    public:
        FindRepository (string repoName) { m_repoName = repoName; }

        bool operator () (Repository repo) {
            if (repo.a_name.compare (m_repoName) == 0)
                return true;
            return false;
        }
};
#include <iostream>
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
    
    FindRepository findRepository ("localhost");
    list<Repository>::iterator match = find_if (e_repository.begin(), e_repository.end(), findRepository);
    
    if (match == e_repository.end()) // entry for localhost repository doesn't exists
    {
        if (xml.str().length() == 0)
            printf ("xml\n");
        
        if (match == e_repository.end())
            printf ("repo\n");
        
        a_useDBus = true;
        Repository r;
        r.a_name = string ("localhost");
        string s;
        s = getenv("HOME"); 
        Path p;
        p.a_path = s;                 r.e_path.push_back(p);
        p.a_path = s + "/.kde";       r.e_path.push_back(p);
        p.a_path = s + "/.gnome2";    r.e_path.push_back(p);
        p.a_path = s + "/.evolution"; r.e_path.push_back(p);
        p.a_path = s + "/.mozilla";   r.e_path.push_back(p);
        p.a_path = s + "/.mozilla-thunderbird";   r.e_path.push_back(p);
        e_repository.push_back(r);
        
        STRIGI_LOG_WARNING ("DaemonConfigurator", "created default config for indexed dirs")
    }
}

void DaemonConfigurator::setIndexedDirs (set<string>& dirs, const string& repositoryName)
{
    FindRepository findRepository (repositoryName);
    
    list<Repository>::iterator match = find_if (e_repository.begin(), e_repository.end(), findRepository);
    
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
    r->e_path.clear();
    
    for (set<string>::const_iterator iter = dirs.begin(); iter != dirs.end(); iter++)
        r->e_path.push_back(*iter);
}

set<string> DaemonConfigurator::getIndexedDirs (const string& repositoryName)
{
    set<string> dirs;
    
    FindRepository findRepository (repositoryName);
    
    list<Repository>::iterator match = find_if (e_repository.begin(), e_repository.end(), findRepository);
    
    if (match == e_repository.end())
    {
        STRIGI_LOG_WARNING ("DaemonConfigurator.getIndexedDirs", "cannot find repository name: |" + repositoryName + "|")
        return dirs;
    }
    
    for (list<Path>::const_iterator iter = match->e_path.begin(); iter != match->e_path.end(); iter++)
        dirs.insert (iter->a_path);
    
    return dirs;
}

void DaemonConfigurator::save()
{
    ofstream f;
    f.open(m_confFile.c_str(), std::ios::binary);
    f << *this;
    f.close();
}


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

#ifndef DAEMONCONFIGURATOR_H
#define DAEMONCONFIGURATOR_H

#include "xsd/strigidaemonconfiguration.h"
#include <strigi/analyzerconfiguration.h>

#include <set>

class DaemonConfigurator : public StrigiDaemonConfiguration {
public:
    // default polling interval is three minutes
    static const int DEFAULT_POLLING_INTERVAL = 180;
    
    explicit DaemonConfigurator (const std::string& confFile);

    void setIndexedDirectories(std::set<std::string>& dirs,
        const std::string& repositoryName = "localhost", bool  merge = false);
    std::set<std::string> getIndexedDirectories(
            const std::string& repositoryName = "localhost");
    bool useDBus() { return a_useDBus; }
    void loadFilteringRules(Strigi::AnalyzerConfiguration& config);
    void saveFilteringRules(const std::vector<std::pair<bool,std::string> >& f);
    void setPollingInterval (unsigned int value,
        const std::string& repositoryName = "localhost");
    unsigned int getPollingInterval(
        const std::string& repositoryName = "localhost");

    void save(const char* file = NULL);
    std::string getWriteableIndexType() const;
    std::string getWriteableIndexDir() const;
    std::list<Repository> getReadOnlyRepositories() const;

private:
    std::string m_confFile;
};

#endif

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

#include "strigidaemonconfiguration.h"

#include <set>

class FilterManager;

class DaemonConfigurator : public StrigiDaemonConfiguration
{
    public:
        DaemonConfigurator (const std::string& confFile);
    
        void setIndexedDirectories (std::set<std::string>& dirs, const std::string& repositoryName = "localhost");
        std::set<std::string> getIndexedDirectories (const std::string& repositoryName = "localhost");
        bool useDBus() { return a_useDBus; }
        void loadFilteringRules (FilterManager* filterManager);
        void saveFilteringRules (FilterManager* filterManager);
        
        void save();
        
    private:
        std::string m_confFile;
};

#endif

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
#ifndef INOTIFYEVENT_H
#define INOTIFYEVENT_H

#include <string>
#include <time.h>
#include <ostream>

class InotifyEvent
{
    public:
        enum Type{ CREATED = 0x01, UPDATED = 0x02, DELETED = 0x04};
        
        InotifyEvent(Type type, std::string path, int wd, time_t t);
        InotifyEvent(InotifyEvent* event);
        ~InotifyEvent();
        
        Type getType() { return m_type; }
        void setType(Type type) { m_type = type; }
        std::string getPath() { return m_path; }
        int getWD () { return m_wd; }
        time_t getTime() { return m_time; }
        
        std::pair< int, std::string> hash() { return std::make_pair (m_wd, m_path); }
        
        friend std::ostream &operator<< (std::ostream &stream, InotifyEvent* event);
        
    protected:
        Type m_type;
        std::string m_path;
        int m_wd;
        time_t m_time;
};

#endif

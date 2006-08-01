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
#include "inotifyevent.h"

InotifyEvent::InotifyEvent(Type type, std::string path, int wd, time_t t)
    : m_type (type),
      m_path (path),
      m_wd (wd),
      m_time (t)
{
}

InotifyEvent::InotifyEvent(InotifyEvent* event)
    : m_type (event->m_type),
      m_path (event->m_path),
      m_wd (event->m_wd),
      m_time (event->m_time)
{
    
}

InotifyEvent::~InotifyEvent()
{
}

std::ostream &operator<< (std::ostream &stream, InotifyEvent* event)
{
    stream << "\ttype: ";
    
    switch (event->getType())
    {
        case InotifyEvent::CREATED:
            stream << "CREATED\n";
            break;
        case InotifyEvent::UPDATED:
            stream << "UPDATED\n";
            break;
        case InotifyEvent::DELETED:
            stream << "DELETED\n";
            break;
    }
    
    stream << "\twd: " << event->getWD() << std::endl;
    stream << "\tpath: " << event->getPath() << std::endl;
    
    const time_t time = event->getTime();
    stream << "\ttime: " << ctime (&time) << std::endl;
    
    return stream;
}

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
#include "event.h"

Event::Event(Type type, const std::string& path)
    : m_type (type),
      m_path (path)
{
    time(&m_time);
}

Event::Event(Type type, const std::string& path, time_t t)
    : m_type (type),
      m_path (path),
      m_time (t)
{
}

Event::Event(Event* event)
    : m_type (event->m_type),
      m_path (event->m_path),
      m_time (event->m_time)
{

}

Event::~Event()
{
}

std::string Event::toString()
{
    std::string out = "type=";

    switch (m_type)
    {
        case Event::CREATED:
            out += "CREATED";
            break;
        case Event::UPDATED:
            out += "UPDATED";
            break;
        case Event::DELETED:
            out +=  "DELETED";
            break;
    }

    out += "; path=";
    out += m_path;

    out += "; time=";
    out += ctime (&m_time);

    return out;
}

std::ostream &operator<< (std::ostream &stream, Event* event)
{
    stream << "\ttype: ";

    switch (event->getType())
    {
        case Event::CREATED:
            stream << "CREATED\n";
            break;
        case Event::UPDATED:
            stream << "UPDATED\n";
            break;
        case Event::DELETED:
            stream << "DELETED\n";
            break;
    }

    stream << "\tpath: " << event->getPath() << std::endl;

    const time_t time = event->getTime();
    stream << "\ttime: " << ctime (&time) << std::endl;

    return stream;
}

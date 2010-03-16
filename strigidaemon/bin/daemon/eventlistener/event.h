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
#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <time.h>
#include <ostream>

/*!
* @class Event
* @brief Class describing a simple file read/write/delete event
*/
class Event
{
    public:
        enum Type{ CREATED = 0x01, UPDATED = 0x02, DELETED = 0x04};

        /*!
        * @param type event type
        * @param path complete path to created/updated/deleted file
        * Creates a new event. m_time is automatically set to current time
        */
        Event(Type type, const std::string& path);

        /*!
        * @param type event type
        * @param path complete path to created/updated/deleted file
        * @param t event creation time
        */
        Event(Type type, const std::string& path, time_t t);
        explicit Event(Event* event);
        ~Event();

        Type getType() { return m_type; }
        void setType(Type type) { m_type = type; }
        const std::string &getPath() { return m_path; }
        time_t getTime() { return m_time; }
        void   setTime(time_t time) {m_time = time;}

        /*!
        * @return a string containing event's resume
        */
        std::string toString();
        friend std::ostream &operator<< (std::ostream &stream, Event* event);

    protected:
        Type m_type; //!< event type
        const std::string m_path; //!< complete path to created/modified/deleted file
        time_t m_time; //!< event creation time
};

#endif

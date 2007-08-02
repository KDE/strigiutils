/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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

#ifndef _DIJON_STRIGIQUERYBUILDER_H
#define _DIJON_STRIGIQUERYBUILDER_H

#include <string>
#include <set>

#include "query.h"
#include "queryparser.h"
#include "XesamQueryBuilder.h"

namespace Dijon
{
    /// A query builder for Strigi::Query.
    class StrigiQueryBuilder : public XesamQueryBuilder
    {
        public:
            /// Builds a query builder for Strigi::Query.
            StrigiQueryBuilder();
            virtual ~StrigiQueryBuilder();

            virtual void on_query(const char *type);

            virtual void on_selection(SelectionType selection,
                                const std::set<std::string> &field_names,
                                const std::vector<std::string> &field_values,
                                SimpleType field_type,
                                const Modifiers &modifiers);

            Strigi::Query get_query(void) const;

            virtual void set_collector(const Collector &collector);
            
        public:
            Strigi::Query m_fullQuery;
            bool m_firstSelection;
            bool m_collectorChanged;

        private:
            /// StrigiQueryBuilder objects cannot be copied.
            StrigiQueryBuilder(const StrigiQueryBuilder &other);
            /// StrigiQueryBuilder objects cannot be copied.
            StrigiQueryBuilder& operator=(const StrigiQueryBuilder &other);

    };
}

#endif // _DIJON_STRIGIQUERYBUILDER_H

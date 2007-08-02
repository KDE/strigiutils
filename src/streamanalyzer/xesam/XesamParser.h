/*
 *  Copyright 2007 Fabrice Colin
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _DIJON_XESAMPARSER_H
#define _DIJON_XESAMPARSER_H

#include <string>

#include "XesamQueryBuilder.h"

namespace Dijon
{
    /// A Xesam parser.
    class XesamParser
    {
    public:
	/// Builds a parser.
	XesamParser() {};
	virtual ~XesamParser() {};

	virtual bool parse(const std::string &xesam_query,
		XesamQueryBuilder &query_builder) = 0;

	virtual bool parse_file(const std::string &xesam_query_file,
		XesamQueryBuilder &query_builder) = 0;

    private:
	/// XesamParser objects cannot be copied.
	XesamParser(const XesamParser &other);
	/// XesamParser objects cannot be copied.
	XesamParser& operator=(const XesamParser &other);

    };
}

#endif // _DIJON_XESAMPARSER_H

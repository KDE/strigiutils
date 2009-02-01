/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#ifndef STRIGI_VARIANT_H
#define STRIGI_VARIANT_H

#include <strigi/strigiconfig.h>
#include <string>
#include <vector>
#include <sstream>

#include <stdlib.h>

namespace Strigi {

class VariantPrivate;
/**
 * Simple inefficient implementation of a variant type as needed by xesam
 **/
class STREAMANALYZER_EXPORT Variant {
public:
    enum Type {b_val, i_val, s_val, as_val, aas_val, u_val};
private:
    VariantPrivate* const p;
public:
    Variant();
    Variant(bool v);
    Variant(int32_t v);
    Variant(uint32_t v);
    Variant(const char* v);
    Variant(const std::string& v);
    Variant(const std::vector<std::string>& v);
    Variant(const std::vector<std::vector<std::string> >& v);
    Variant(const Variant& v);
    ~Variant();
    Type type() const;
    const Variant& operator=(bool v);
    const Variant& operator=(int32_t v);
    const Variant& operator=(uint32_t v);
    const Variant& operator=(const char* v);
    const Variant& operator=(const std::string& v);
    const Variant& operator=(const std::vector<std::string>& v);
    const Variant& operator=(const std::vector<std::vector<std::string> >& v);
    const Variant& operator=(const Variant& v);
    bool isValid() const;
    bool b() const;
    int32_t i() const;
    uint32_t u() const;
    std::string s() const;
    std::vector<std::string> as() const;
    std::vector<std::vector<std::string> > aas() const;
};

}

#endif

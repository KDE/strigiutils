// file      : xsd/cxx/tree/type-name-map.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#if defined(XSD_CXX_TREE_USE_CHAR) || !defined(XSD_CXX_TREE_USE_WCHAR)

#ifndef XSD_CXX_TREE_TYPE_NAME_MAP_IXX_CHAR
#define XSD_CXX_TREE_TYPE_NAME_MAP_IXX_CHAR

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <>
      inline type_name_map<char>::
      type_name_map ()
      {
        // Register factories for built-in non-fundamental types.
        //

        std::basic_string<char> xsd ("http://www.w3.org/2001/XMLSchema");


        // anyType and anySimpleType.
        //
        register_type (
          typeid (type),
          qualified_name<char> ("anyType", xsd),
          &bits::serializer<char, type>::impl);

        register_type (
          typeid (simple_type),
          qualified_name<char> ("anySimpleType", xsd),
          &bits::serializer<char, simple_type>::impl);


        // Strings
        //
        register_type (
          typeid (string<char>),
          qualified_name<char> ("string", xsd),
          &bits::serializer<char, string<char> >::impl);

        register_type (
          typeid (normalized_string<char>),
          qualified_name<char> ("normalizedString", xsd),
          &bits::serializer<char, normalized_string<char> >::impl);

        register_type (
          typeid (token<char>),
          qualified_name<char> ("token", xsd),
          &bits::serializer<char, token<char> >::impl);

        register_type (
          typeid (name<char>),
          qualified_name<char> ("Name", xsd),
          &bits::serializer<char, name<char> >::impl);

        register_type (
          typeid (nmtoken<char>),
          qualified_name<char> ("NMTOKEN", xsd),
          &bits::serializer<char, nmtoken<char> >::impl);

        register_type (
          typeid (nmtokens<char>),
          qualified_name<char> ("NMTOKENS", xsd),
          &bits::serializer<char, nmtokens<char> >::impl);

        register_type (
          typeid (ncname<char>),
          qualified_name<char> ("NCName", xsd),
          &bits::serializer<char, ncname<char> >::impl);

        register_type (
          typeid (language<char>),
          qualified_name<char> ("language", xsd),
          &bits::serializer<char, language<char> >::impl);


        // Qualified name.
        //
        register_type (
          typeid (qname<char>),
          qualified_name<char> ("QName", xsd),
          &bits::serializer<char, qname<char> >::impl);


        // ID/IDREF.
        //
        register_type (
          typeid (id<char>),
          qualified_name<char> ("ID", xsd),
          &bits::serializer<char, id<char> >::impl);

        register_type (
          typeid (idref<type, char>),
          qualified_name<char> ("IDREF", xsd),
          &bits::serializer<char, idref<type, char> >::impl);

        register_type (
          typeid (idrefs<type, char>),
          qualified_name<char> ("IDREFS", xsd),
          &bits::serializer<char, idrefs<type, char> >::impl);


        // URI.
        //
        register_type (
          typeid (uri<char>),
          qualified_name<char> ("anyURI", xsd),
          &bits::serializer<char, uri<char> >::impl);


        // Binary.
        //
        register_type (
          typeid (base64_binary<char>),
          qualified_name<char> ("base64Binary", xsd),
          &bits::serializer<char, base64_binary<char> >::impl);

        register_type (
          typeid (hex_binary<char>),
          qualified_name<char> ("hexBinary", xsd),
          &bits::serializer<char, hex_binary<char> >::impl);


        // Date/time.
        //
        register_type (
          typeid (date<char>),
          qualified_name<char> ("date", xsd),
          &bits::serializer<char, date<char> >::impl);

        register_type (
          typeid (date_time<char>),
          qualified_name<char> ("dateTime", xsd),
          &bits::serializer<char, date_time<char> >::impl);

        register_type (
          typeid (duration<char>),
          qualified_name<char> ("duration", xsd),
          &bits::serializer<char, duration<char> >::impl);

        register_type (
          typeid (day<char>),
          qualified_name<char> ("gDay", xsd),
          &bits::serializer<char, day<char> >::impl);

        register_type (
          typeid (month<char>),
          qualified_name<char> ("gMonth", xsd),
          &bits::serializer<char, month<char> >::impl);

        register_type (
          typeid (month_day<char>),
          qualified_name<char> ("gMonthDay", xsd),
          &bits::serializer<char, month_day<char> >::impl);

        register_type (
          typeid (year<char>),
          qualified_name<char> ("gYear", xsd),
          &bits::serializer<char, year<char> >::impl);

        register_type (
          typeid (year_month<char>),
          qualified_name<char> ("gYearMonth", xsd),
          &bits::serializer<char, year_month<char> >::impl);

        register_type (
          typeid (time<char>),
          qualified_name<char> ("time", xsd),
          &bits::serializer<char, time<char> >::impl);


        // Entity.
        //
        register_type (
          typeid (entity<char>),
          qualified_name<char> ("ENTITY", xsd),
          &bits::serializer<char, entity<char> >::impl);

        register_type (
          typeid (entities<char>),
          qualified_name<char> ("ENTITIES", xsd),
          &bits::serializer<char, entities<char> >::impl);
      }
    }
  }
}

#endif // XSD_CXX_TREE_TYPE_NAME_MAP_IXX_CHAR
#endif // XSD_CXX_TREE_USE_CHAR


#if defined(XSD_CXX_TREE_USE_WCHAR) || !defined(XSD_CXX_TREE_USE_CHAR)

#ifndef XSD_CXX_TREE_TYPE_NAME_MAP_IXX_WCHAR
#define XSD_CXX_TREE_TYPE_NAME_MAP_IXX_WCHAR

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <>
      inline type_name_map<wchar_t>::
      type_name_map ()
      {
        // Register factories for built-in non-fundamental types.
        //

        std::basic_string<wchar_t> xsd (L"http://www.w3.org/2001/XMLSchema");


        // anyType and anySimpleType.
        //
        register_type (
          typeid (type),
          qualified_name<wchar_t> (L"anyType", xsd),
          &bits::serializer<wchar_t, type>::impl);

        register_type (
          typeid (simple_type),
          qualified_name<wchar_t> (L"anySimpleType", xsd),
          &bits::serializer<wchar_t, simple_type>::impl);


        // Strings
        //
        register_type (
          typeid (string<wchar_t>),
          qualified_name<wchar_t> (L"string", xsd),
          &bits::serializer<wchar_t, string<wchar_t> >::impl);

        register_type (
          typeid (normalized_string<wchar_t>),
          qualified_name<wchar_t> (L"normalizedString", xsd),
          &bits::serializer<wchar_t, normalized_string<wchar_t> >::impl);

        register_type (
          typeid (token<wchar_t>),
          qualified_name<wchar_t> (L"token", xsd),
          &bits::serializer<wchar_t, token<wchar_t> >::impl);

        register_type (
          typeid (name<wchar_t>),
          qualified_name<wchar_t> (L"Name", xsd),
          &bits::serializer<wchar_t, name<wchar_t> >::impl);

        register_type (
          typeid (nmtoken<wchar_t>),
          qualified_name<wchar_t> (L"NMTOKEN", xsd),
          &bits::serializer<wchar_t, nmtoken<wchar_t> >::impl);

        register_type (
          typeid (nmtokens<wchar_t>),
          qualified_name<wchar_t> (L"NMTOKENS", xsd),
          &bits::serializer<wchar_t, nmtokens<wchar_t> >::impl);

        register_type (
          typeid (ncname<wchar_t>),
          qualified_name<wchar_t> (L"NCName", xsd),
          &bits::serializer<wchar_t, ncname<wchar_t> >::impl);

        register_type (
          typeid (language<wchar_t>),
          qualified_name<wchar_t> (L"language", xsd),
          &bits::serializer<wchar_t, language<wchar_t> >::impl);


        // Qualified name.
        //
        register_type (
          typeid (qname<wchar_t>),
          qualified_name<wchar_t> (L"QName", xsd),
          &bits::serializer<wchar_t, qname<wchar_t> >::impl);


        // ID/IDREF.
        //
        register_type (
          typeid (id<wchar_t>),
          qualified_name<wchar_t> (L"ID", xsd),
          &bits::serializer<wchar_t, id<wchar_t> >::impl);

        register_type (
          typeid (idref<type, wchar_t>),
          qualified_name<wchar_t> (L"IDREF", xsd),
          &bits::serializer<wchar_t, idref<type, wchar_t> >::impl);

        register_type (
          typeid (idrefs<type, wchar_t>),
          qualified_name<wchar_t> (L"IDREFS", xsd),
          &bits::serializer<wchar_t, idrefs<type, wchar_t> >::impl);


        // URI.
        //
        register_type (
          typeid (uri<wchar_t>),
          qualified_name<wchar_t> (L"anyURI", xsd),
          &bits::serializer<wchar_t, uri<wchar_t> >::impl);


        // Binary.
        //
        register_type (
          typeid (base64_binary<wchar_t>),
          qualified_name<wchar_t> (L"base64Binary", xsd),
          &bits::serializer<wchar_t, base64_binary<wchar_t> >::impl);

        register_type (
          typeid (hex_binary<wchar_t>),
          qualified_name<wchar_t> (L"hexBinary", xsd),
          &bits::serializer<wchar_t, hex_binary<wchar_t> >::impl);


        // Date/time.
        //
        register_type (
          typeid (date<wchar_t>),
          qualified_name<wchar_t> (L"date", xsd),
          &bits::serializer<wchar_t, date<wchar_t> >::impl);

        register_type (
          typeid (date_time<wchar_t>),
          qualified_name<wchar_t> (L"dateTime", xsd),
          &bits::serializer<wchar_t, date_time<wchar_t> >::impl);

        register_type (
          typeid (duration<wchar_t>),
          qualified_name<wchar_t> (L"duration", xsd),
          &bits::serializer<wchar_t, duration<wchar_t> >::impl);

        register_type (
          typeid (day<wchar_t>),
          qualified_name<wchar_t> (L"gDay", xsd),
          &bits::serializer<wchar_t, day<wchar_t> >::impl);

        register_type (
          typeid (month<wchar_t>),
          qualified_name<wchar_t> (L"gMonth", xsd),
          &bits::serializer<wchar_t, month<wchar_t> >::impl);

        register_type (
          typeid (month_day<wchar_t>),
          qualified_name<wchar_t> (L"gMonthDay", xsd),
          &bits::serializer<wchar_t, month_day<wchar_t> >::impl);

        register_type (
          typeid (year<wchar_t>),
          qualified_name<wchar_t> (L"gYear", xsd),
          &bits::serializer<wchar_t, year<wchar_t> >::impl);

        register_type (
          typeid (year_month<wchar_t>),
          qualified_name<wchar_t> (L"gYearMonth", xsd),
          &bits::serializer<wchar_t, year_month<wchar_t> >::impl);

        register_type (
          typeid (time<wchar_t>),
          qualified_name<wchar_t> (L"time", xsd),
          &bits::serializer<wchar_t, time<wchar_t> >::impl);


        // Entity.
        //
        register_type (
          typeid (entity<wchar_t>),
          qualified_name<wchar_t> (L"ENTITY", xsd),
          &bits::serializer<wchar_t, entity<wchar_t> >::impl);

        register_type (
          typeid (entities<wchar_t>),
          qualified_name<wchar_t> (L"ENTITIES", xsd),
          &bits::serializer<wchar_t, entities<wchar_t> >::impl);
      }
    }
  }
}

#endif // XSD_CXX_TREE_TYPE_NAME_MAP_IXX_WCHAR
#endif // XSD_CXX_TREE_USE_WCHAR

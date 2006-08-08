// file      : xsd/cxx/tree/type-factory-map.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#if defined(XSD_CXX_TREE_USE_CHAR) || !defined(XSD_CXX_TREE_USE_WCHAR)

#ifndef XSD_CXX_TREE_TYPE_FACTORY_MAP_IXX_CHAR
#define XSD_CXX_TREE_TYPE_FACTORY_MAP_IXX_CHAR

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <>
      inline type_factory_map<char>::
      type_factory_map ()
      {
        // Register factories for built-in non-fundamental types.
        //

        std::basic_string<char> xsd ("http://www.w3.org/2001/XMLSchema");


        // anyType and anySimpleType.
        //
        register_type (
          "anyType " + xsd,
          &factory_impl<char, type>);

        register_type (
          "anySimpleType " + xsd,
          &factory_impl<char, simple_type>);


        // Strings
        //
        register_type (
          "string " + xsd,
          &factory_impl<char, string<char> >);

        register_type (
          "normalizedString " + xsd,
          &factory_impl<char, normalized_string<char> >);

        register_type (
          "token " + xsd,
          &factory_impl<char, token<char> >);

        register_type (
          "Name " + xsd,
          &factory_impl<char, name<char> >);

        register_type (
          "NMTOKEN " + xsd,
          &factory_impl<char, nmtoken<char> >);

        register_type (
          "NMTOKENS " + xsd,
          &factory_impl<char, nmtokens<char> >);

        register_type (
          "NCName " + xsd,
          &factory_impl<char, ncname<char> >);

        register_type (
          "language " + xsd,
          &factory_impl<char, language<char> >);


        // Qualified name.
        //
        register_type (
          "QName " + xsd,
          &factory_impl<char, qname<char> >);


        // ID/IDREF.
        //
        register_type (
          "ID " + xsd,
          &factory_impl<char, id<char> >);

        register_type (
          "IDREF " + xsd,
          &factory_impl<char, idref<type, char> >);

        register_type (
          "IDREFS " + xsd,
          &factory_impl<char, idrefs<type, char> >);


        // URI.
        //
        register_type (
          "anyURI " + xsd,
          &factory_impl<char, uri<char> >);


        // Binary.
        //
        register_type (
          "base64Binary " + xsd,
          &factory_impl<char, base64_binary<char> >);

        register_type (
          "hexBinary " + xsd,
          &factory_impl<char, hex_binary<char> >);


        // Date/time.
        //
        register_type (
          "date " + xsd,
          &factory_impl<char, date<char> >);

        register_type (
          "dateTime " + xsd,
          &factory_impl<char, date_time<char> >);

        register_type (
          "duration " + xsd,
          &factory_impl<char, duration<char> >);

        register_type (
          "gDay " + xsd,
          &factory_impl<char, day<char> >);

        register_type (
          "gMonth " + xsd,
          &factory_impl<char, month<char> >);

        register_type (
          "gMonthDay " + xsd,
          &factory_impl<char, month_day<char> >);

        register_type (
          "gYear " + xsd,
          &factory_impl<char, year<char> >);

        register_type (
          "gYearMonth " + xsd,
          &factory_impl<char, year_month<char> >);

        register_type (
          "time " + xsd,
          &factory_impl<char, time<char> >);


        // Entity.
        //
        register_type (
          "ENTITY " + xsd,
          &factory_impl<char, entity<char> >);

        register_type (
          "ENTITIES " + xsd,
          &factory_impl<char, entities<char> >);
      }
    }
  }
}

#endif // XSD_CXX_TREE_TYPE_FACTORY_MAP_IXX_CHAR
#endif // XSD_CXX_TREE_USE_CHAR


#if defined(XSD_CXX_TREE_USE_WCHAR) || !defined(XSD_CXX_TREE_USE_CHAR)

#ifndef XSD_CXX_TREE_TYPE_FACTORY_MAP_IXX_WCHAR
#define XSD_CXX_TREE_TYPE_FACTORY_MAP_IXX_WCHAR

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <>
      inline type_factory_map<wchar_t>::
      type_factory_map ()
      {
        // Register factories for built-in non-fundamental types.
        //

        std::basic_string<wchar_t> xsd (L"http://www.w3.org/2001/XMLSchema");


        // anyType and anySimpleType.
        //
        register_type (
          L"anyType " + xsd,
          &factory_impl<wchar_t, type>);

        register_type (
          L"anySimpleType " + xsd,
          &factory_impl<wchar_t, simple_type>);


        // Strings
        //
        register_type (
          L"string " + xsd,
          &factory_impl<wchar_t, string<wchar_t> >);

        register_type (
          L"normalizedString " + xsd,
          &factory_impl<wchar_t, normalized_string<wchar_t> >);

        register_type (
          L"token " + xsd,
          &factory_impl<wchar_t, token<wchar_t> >);

        register_type (
          L"Name " + xsd,
          &factory_impl<wchar_t, name<wchar_t> >);

        register_type (
          L"NMTOKEN " + xsd,
          &factory_impl<wchar_t, nmtoken<wchar_t> >);

        register_type (
          L"NMTOKENS " + xsd,
          &factory_impl<wchar_t, nmtokens<wchar_t> >);

        register_type (
          L"NCName " + xsd,
          &factory_impl<wchar_t, ncname<wchar_t> >);

        register_type (
          L"language " + xsd,
          &factory_impl<wchar_t, language<wchar_t> >);


        // Qualified name.
        //
        register_type (
          L"QName " + xsd,
          &factory_impl<wchar_t, qname<wchar_t> >);


        // ID/IDREF.
        //
        register_type (
          L"ID " + xsd,
          &factory_impl<wchar_t, id<wchar_t> >);

        register_type (
          L"IDREF " + xsd,
          &factory_impl<wchar_t, idref<type, wchar_t> >);

        register_type (
          L"IDREFS " + xsd,
          &factory_impl<wchar_t, idrefs<type, wchar_t> >);


        // URI.
        //
        register_type (
          L"anyURI " + xsd,
          &factory_impl<wchar_t, uri<wchar_t> >);


        // Binary.
        //
        register_type (
          L"base64Binary " + xsd,
          &factory_impl<wchar_t, base64_binary<wchar_t> >);

        register_type (
          L"hexBinary " + xsd,
          &factory_impl<wchar_t, hex_binary<wchar_t> >);


        // Date/time.
        //
        register_type (
          L"date " + xsd,
          &factory_impl<wchar_t, date<wchar_t> >);

        register_type (
          L"dateTime " + xsd,
          &factory_impl<wchar_t, date_time<wchar_t> >);

        register_type (
          L"duration " + xsd,
          &factory_impl<wchar_t, duration<wchar_t> >);

        register_type (
          L"gDay " + xsd,
          &factory_impl<wchar_t, day<wchar_t> >);

        register_type (
          L"gMonth " + xsd,
          &factory_impl<wchar_t, month<wchar_t> >);

        register_type (
          L"gMonthDay " + xsd,
          &factory_impl<wchar_t, month_day<wchar_t> >);

        register_type (
          L"gYear " + xsd,
          &factory_impl<wchar_t, year<wchar_t> >);

        register_type (
          L"gYearMonth " + xsd,
          &factory_impl<wchar_t, year_month<wchar_t> >);

        register_type (
          L"time " + xsd,
          &factory_impl<wchar_t, time<wchar_t> >);


        // Entity.
        //
        register_type (
          L"ENTITY " + xsd,
          &factory_impl<wchar_t, entity<wchar_t> >);

        register_type (
          L"ENTITIES " + xsd,
          &factory_impl<wchar_t, entities<wchar_t> >);
      }
    }
  }
}

#endif // XSD_CXX_TREE_TYPE_FACTORY_MAP_IXX_WCHAR
#endif // XSD_CXX_TREE_USE_WCHAR

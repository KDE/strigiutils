// file      : xsd/cxx/parser/elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_PARSER_ELEMENTS_HXX
#define XSD_CXX_PARSER_ELEMENTS_HXX

#include <string>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      //
      //
      template <typename C>
      struct parser_base
      {
        virtual
        ~parser_base ();

        virtual void
        pre () = 0;

        virtual void
        _pre () = 0;

        virtual void
        _start_element (const std::basic_string<C>& ns,
                        const std::basic_string<C>& name) = 0;

        virtual void
        _end_element (const std::basic_string<C>& ns,
                      const std::basic_string<C>& name) = 0;

        virtual void
        _attribute (const std::basic_string<C>& ns,
                    const std::basic_string<C>& name,
                    const std::basic_string<C>& value) = 0;

        virtual void
        _characters (const std::basic_string<C>&) = 0;

        virtual void
        _post () = 0;
      };


      //
      //
      template <typename X, typename C>
      struct parser: virtual parser_base<C>
      {
        virtual X
        post () = 0;
      };

      template <typename C>
      struct parser<void, C>: virtual parser_base<C>
      {
        virtual void
        post ()
        {
        }
      };
    }
  }
}

#include <xsd/cxx/parser/elements.txx>

#endif  // XSD_CXX_PARSER_ELEMENTS_HXX

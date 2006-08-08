// file      : xsd/cxx/parser/non-validating/parser.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_PARSER_NON_VALIDATING_PARSER_HXX
#define XSD_CXX_PARSER_NON_VALIDATING_PARSER_HXX

#include <deque>
#include <string>
#include <cstdlib> // std::size_t

#include <xsd/cxx/parser/elements.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        //
        //
        template <typename C>
        struct empty_content: virtual parser_base<C>
        {
          // These functions are called when wildcard content
          // is encountered. Use them to handle mixed content
          // models, any/anyAttribute, and anyType/anySimpleType.
          // By default these functions do nothing.
          //
          virtual void
          _start_any_element (const std::basic_string<C>& ns,
                              const std::basic_string<C>& name);

          virtual void
          _end_any_element (const std::basic_string<C>& ns,
                            const std::basic_string<C>& name);

          virtual void
          _any_attribute (const std::basic_string<C>& ns,
                          const std::basic_string<C>& name,
                          const std::basic_string<C>& value);

          virtual void
          _any_characters (const std::basic_string<C>&);


          //
          //
          virtual bool
          _start_element_impl (const std::basic_string<C>&,
                               const std::basic_string<C>&);

          virtual bool
          _end_element_impl (const std::basic_string<C>&,
                             const std::basic_string<C>&);

          virtual bool
          _attribute_impl (const std::basic_string<C>&,
                           const std::basic_string<C>&,
                           const std::basic_string<C>&);

          virtual bool
          _characters_impl (const std::basic_string<C>&);


          //
          //
          virtual void
          _start_element (const std::basic_string<C>& ns,
                          const std::basic_string<C>& name);

          virtual void
          _end_element (const std::basic_string<C>& ns,
                        const std::basic_string<C>& name);

          virtual void
          _attribute (const std::basic_string<C>& ns,
                      const std::basic_string<C>& name,
                      const std::basic_string<C>& value);

          virtual void
          _characters (const std::basic_string<C>& s);


          //
          //
          virtual void
          pre ();

          virtual void
          _pre ();

          virtual void
          _post ();
        };


        //
        //
        template <typename C>
        struct simple_type: virtual empty_content<C>
        {
        };


        // @@ Not used in the generated code at moment.
        //
        template <typename C>
        struct simple_content: virtual simple_type<C>
        {
        };


        //
        //
        template <typename C>
        struct complex_content: virtual simple_content<C>
        {
          //
          //
          virtual void
          _start_element (const std::basic_string<C>& ns,
                          const std::basic_string<C>& name);

          virtual void
          _end_element (const std::basic_string<C>& ns,
                        const std::basic_string<C>& name);

          virtual void
          _attribute (const std::basic_string<C>& ns,
                      const std::basic_string<C>& name,
                      const std::basic_string<C>& value);

          virtual void
          _characters (const std::basic_string<C>&);


          //
          //
          virtual void
          _pre ();

          virtual void
          _post ();

        protected:
          struct state
          {
            state ()
                : depth_ (0), parser_ (0)
            {
            }

            std::size_t depth_;
            parser_base<C>* parser_;
          };

          std::deque<state> context_;
        };
      }
    }
  }
}

#include <xsd/cxx/parser/non-validating/parser.txx>

#endif  // XSD_CXX_PARSER_NON_VALIDATING_PARSER_HXX

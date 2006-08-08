// file      : xsd/cxx/parser/validating/parser.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_PARSER_VALIDATING_PARSER_HXX
#define XSD_CXX_PARSER_VALIDATING_PARSER_HXX

#include <deque>
#include <stack>
#include <string>
#include <cstdlib> // std::size_t

#include <xsd/cxx/parser/elements.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
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
          _start_element (const std::basic_string<C>&,
                          const std::basic_string<C>&);

          virtual void
          _end_element (const std::basic_string<C>&,
                        const std::basic_string<C>&);

          virtual void
          _attribute (const std::basic_string<C>&,
                      const std::basic_string<C>&,
                      const std::basic_string<C>&);

          virtual void
          _characters (const std::basic_string<C>&);


          //
          //
          virtual void
          _expected_element (const std::basic_string<C>& encountered_ns,
                             const std::basic_string<C>& encountered_name);

          virtual void
          _expected_element (const std::basic_string<C>& encountered_ns,
                             const std::basic_string<C>& encountered_name,
                             const std::basic_string<C>& expected_ns,
                             const std::basic_string<C>& expected_name);

          virtual void
          _unexpected_element (const std::basic_string<C>& ns,
                               const std::basic_string<C>& name);

          virtual void
          _unexpected_attribute (const std::basic_string<C>& ns,
                                 const std::basic_string<C>& name,
                                 const std::basic_string<C>& value);

          virtual void
          _unexpected_characters (const std::basic_string<C>&);


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

          virtual void
          _pre_impl ();

          virtual void
          _post_impl ();

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


        //
        //
        template <typename T, typename C>
        struct state_stack
        {
          typedef void (T::*func_type) (unsigned long&,
                                        unsigned long&,
                                        const std::basic_string<C>&,
                                        const std::basic_string<C>&,
                                        bool);

          struct state_type
          {
            state_type (func_type f, unsigned long s, unsigned long c)
                : function (f), state (s), count (c)
            {
            }

            func_type function;
            unsigned long state;
            unsigned long count;
          };

        public:
          void
          push (func_type f, unsigned long s, unsigned long c)
          {
            stack_.top ().push (state_type (f, s, c));
          }

          void
          pop ()
          {
            stack_.top ().pop ();
          }

          state_type&
          top ()
          {
            return stack_.top ().top ();
          }

        public:
          void
          push_stack ()
          {
            stack_.push (std::stack<state_type> ());
          }

          void
          pop_stack ()
          {
            stack_.pop ();
          }

        private:
          std::stack<std::stack<state_type> > stack_;
        };

        template <unsigned long N>
        struct all_count
        {
          struct counter
          {
            counter ()
            {
              for (unsigned long i (0); i < N; ++i)
                data_[i] = 0;
            }

            unsigned char data_[N];
          };

        public:
          void
          push ()
          {
            stack_.push (counter ());
          }

          void
          pop ()
          {
            stack_.pop ();
          }

          unsigned char*
          top ()
          {
            return stack_.top ().data_;
          }

        private:
          std::stack<counter> stack_;
        };
      }
    }
  }
}

#include <xsd/cxx/parser/validating/parser.txx>

#endif  // XSD_CXX_PARSER_VALIDATING_PARSER_HXX

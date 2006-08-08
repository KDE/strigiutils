// file      : xsd/cxx/parser/non-validating/types.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_PARSER_NON_VALIDATING_TYPES_HXX
#define XSD_CXX_PARSER_NON_VALIDATING_TYPES_HXX

#include <sstream>

#include <xsd/cxx/parser/non-validating/parser.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {
        // 8-bit
        //

        template <typename C>
        struct byte: virtual parser<signed char, C>,
                     virtual simple_type<C>
        {
          typedef signed char type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            short t;
            is >> t;

            v_ = static_cast<type> (t);

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        template <typename C>
        struct unsigned_byte: virtual parser<unsigned char, C>,
                              virtual simple_type<C>
        {
          typedef unsigned char type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            unsigned short t;
            is >> t;

            v_ = static_cast<type> (t);

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        // 16-bit
        //

        template <typename C>
        struct short_: virtual parser<short, C>,
                       virtual simple_type<C>
        {
          typedef short type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        template <typename C>
        struct unsigned_short: virtual parser<unsigned short, C>,
                               virtual simple_type<C>
        {
          typedef unsigned short type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        // 32-bit
        //

        template <typename C>
        struct int_: virtual parser<int, C>,
                     virtual simple_type<C>
        {
          typedef int type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        template <typename C>
        struct unsigned_int: virtual parser<unsigned int, C>,
                             virtual simple_type<C>
        {
          typedef unsigned int type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        // 64-bit
        //

        template <typename C>
        struct long_: virtual parser<long long, C>,
                      virtual simple_type<C>
        {
          typedef long long type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        template <typename C>
        struct unsigned_long: virtual parser<unsigned long long, C>,
                              virtual simple_type<C>
        {
          typedef unsigned long long type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        // Boolean.
        //

        namespace bits
        {
          template<typename C>
          const C*
          true_ ();

          template<typename C>
          const C*
          one ();
        }

        template <typename C>
        struct boolean: virtual parser<bool, C>,
                        virtual simple_type<C>
        {
          typedef bool type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            v_ = (s == bits::true_<C> ()) || (s == bits::one<C> ());

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        // Floats.
        //

        template <typename C>
        struct float_: virtual parser<float, C>,
                       virtual simple_type<C>
        {
          typedef float type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        template <typename C>
        struct double_: virtual parser<double, C>,
                        virtual simple_type<C>
        {
          typedef double type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };

        template <typename C>
        struct decimal: virtual parser<long double, C>,
                        virtual simple_type<C>
        {
          typedef long double type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            std::basic_istringstream<C> is (s);

            is >> v_;

            return true;
          }

          virtual type
          post ()
          {
            return v_;
          }

        private:
          type v_;
        };


        // String.
        //

        template <typename C>
        struct string: virtual parser<std::basic_string<C>, C>,
                       virtual simple_type<C>
        {
          typedef std::basic_string<C> type;

          virtual bool
          _characters_impl (const std::basic_string<C>& s)
          {
            s_ = s;
            return true;
          }

          virtual type
          post ()
          {
            return s_;
          }

        private:
          type s_;
        };


        // Template for xsd:list.
        //

        template <typename X, typename C>
        struct list: virtual simple_type<C>
        {
          list ()
              : item_ (0)
          {
          }

          // Parser hooks. Override them in your implementation.
          //
          virtual void
          item (const X&)
          {
          }

          // Parser construction API.
          //
          void
          item_parser (parser<X, C>& item)
          {
            item_ = &item;
          }

          void
          parsers (parser<X, C>& item)
          {
            item_ = &item;
          }

          // Implementation.
          //

          virtual bool
          _characters_impl (const std::basic_string<C>& s);

        private:
          parser<X, C>* item_;
        };


        // Specialization for void.
        //
        template <typename C>
        struct list<void, C>: virtual simple_type<C>
        {
          list ()
              : item_ (0)
          {
          }

          // Parser hooks. Override them in your implementation.
          //
          virtual void
          item ()
          {
          }

          // Parser construction API.
          //
          void
          item_parser (parser<void, C>& item)
          {
            item_ = &item;
          }

          void
          parsers (parser<void, C>& item)
          {
            item_ = &item;
          }

          // Implementation.
          //

          virtual bool
          _characters_impl (const std::basic_string<C>& s);

        private:
          parser<void, C>* item_;
        };
      }
    }
  }
}

#include <xsd/cxx/parser/non-validating/types.txx>

#endif  // XSD_CXX_PARSER_NON_VALIDATING_TYPES_HXX

#include <xsd/cxx/parser/non-validating/types.ixx>

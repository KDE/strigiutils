// file      : xsd/cxx/tree/traits.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_TRAITS_HXX
#define XSD_CXX_TREE_TRAITS_HXX

// Do not include this file directly. Rather include elements.hxx.
//

#include <sstream>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // bool
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

      template <>
      struct traits<bool>
      {
        typedef bool type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          return (s == bits::true_<C> ()) || (s == bits::one<C> ());
        }
      };


      // 8 bit
      //

      template <>
      struct traits<signed char>
      {
        typedef signed char type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          short t;
          is >> t;

          return static_cast<type> (t);
        }
      };


      template <>
      struct traits<unsigned char>
      {
        typedef unsigned char type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          unsigned short t;
          is >> t;

          return static_cast<type> (t);
        }
      };


      // 16 bit
      //

      template <>
      struct traits<short>
      {
        typedef short type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <>
      struct traits<unsigned short>
      {
        typedef unsigned short type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      // 32 bit
      //

      template <>
      struct traits<int>
      {
        typedef int type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <>
      struct traits<unsigned int>
      {
        typedef unsigned int type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      // 64 bit
      //

      template <>
      struct traits<long long>
      {
        typedef long long type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <>
      struct traits<unsigned long long>
      {
        typedef unsigned long long type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      // floating point
      //

      template <>
      struct traits<float>
      {
        typedef float type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <>
      struct traits<double>
      {
        typedef double type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };


      template <>
      struct traits<long double>
      {
        typedef long double type;

        template <typename C>
        static type
        create (const xml::dom::element<C>& e, flags f, tree::type* c)
        {
          return create (e.value (), f, c);
        }

        template <typename C>
        static type
        create (const xml::dom::attribute<C>& a, flags f, tree::type* c)
        {
          return create (a.value (), f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s,
                const xml::dom::element<C>&,
                flags f,
                tree::type* c)
        {
          return create (s, f, c);
        }

        template <typename C>
        static type
        create (const std::basic_string<C>& s, flags, tree::type*)
        {
          std::basic_istringstream<C> is (s);

          type t;
          is >> t;

          return t;
        }
      };
    }
  }
}

#endif  // XSD_CXX_TREE_TRAITS_HXX

#include <xsd/cxx/tree/traits.ixx>

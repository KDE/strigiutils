// file      : xsd/cxx/tree/std-ostream-operators.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_STD_OSTREAM_OPERATORS_HXX
#define XSD_CXX_TREE_STD_OSTREAM_OPERATORS_HXX

#include <ostream>

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/types.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // type
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const type&)
      {
        return os;
      }


      // simple_type
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const simple_type&)
      {
        return os;
      }


      // fundamental_base
      //
      template <typename C, typename X>
      inline
      std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, fundamental_base<X> x)
      {
        const X& r (x);
        return os << r;
      }


      // optional
      //

      // See std-ostream-operators.ixx for operator<< (basic_ostream, optional)
      //

      // sequence
      //

      // This is an xsd:list-style format (space-separated).
      //
      template <typename C, typename X, bool fund>
      std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const sequence<X, fund>& v)
      {
        for (typename sequence<X, fund>::const_iterator
               b (v.begin ()), e (v.end ()), i (b); i != e; ++i)
        {
          if (i != b)
            os << C (' ');

          os << *i;
        }

        return os;
      }


      // Operators for built-in types.
      //


      // string
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const string<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // normalized_string
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const normalized_string<C>& v)
      {
        const string<C>& r (v);
        return os << r;
      }


      // token
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const token<C>& v)
      {
        const normalized_string<C>& r (v);
        return os << r;
      }


      // nmtoken
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const nmtoken<C>& v)
      {
        const token<C>& r (v);
        return os << r;
      }


      // nmtokens
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const nmtokens<C>& v)
      {
        const sequence<nmtoken<C> >& r (v);
        return os << r;
      }


      // name
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const name<C>& v)
      {
        const token<C>& r (v);
        return os << r;
      }


      // ncname
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const ncname<C>& v)
      {
        const name<C>& r (v);
        return os << r;
      }


      // language
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const language<C>& v)
      {
        const token<C>& r (v);
        return os << r;
      }


      // id
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const id<C>& v)
      {
        const ncname<C>& r (v);
        return os << r;
      }


      // idref
      //
      template <typename X, typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const idref<X, C>& v)
      {
        const ncname<C>& r (v);
        return os << r;
      }


      // idrefs
      //
      template <typename X, typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const idrefs<X, C>& v)
      {
        const sequence<idref<X, C> >& r (v);
        return os << r;
      }


      // uri
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const uri<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // qname
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const qname<C>& n)
      {
        return os << n.namespace_ () << C ('#') << n.name ();
      }


      // base64_binary
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const base64_binary<C>& v)
      {
        return os << v.encode ();
      }


      // hex_binary
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const hex_binary<C>& v)
      {
        return os << v.encode ();
      }


      // date
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const date<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // date_time
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const date_time<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // duration
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const duration<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // day
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const day<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // month
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const month<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // month_day
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const month_day<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // year
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const year<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // year_month
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const year_month<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // time
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const time<C>& v)
      {
        const std::basic_string<C>& r (v);
        return os << r;
      }


      // entity
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const entity<C>& v)
      {
        const ncname<C>& r (v);
        return os << r;
      }


      // entities
      //
      template <typename C>
      inline std::basic_ostream<C>&
      operator<< (std::basic_ostream<C>& os, const entities<C>& v)
      {
        const sequence<entity<C> >& r (v);
        return os << r;
      }
    }
  }
}

#endif  // XSD_CXX_TREE_STD_OSTREAM_OPERATORS_HXX

#include <xsd/cxx/tree/std-ostream-operators.ixx>

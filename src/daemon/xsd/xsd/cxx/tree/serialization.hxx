// file      : xsd/cxx/tree/serialization.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_SERIALIZATION_HXX
#define XSD_CXX_TREE_SERIALIZATION_HXX

#include <sstream>

#include <xsd/cxx/xml/dom/elements.hxx>
#include <xsd/cxx/xml/dom/serialization.hxx>

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/types.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // Insertion operators for type.
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>&, const type&)
      {
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>&, const type&)
      {
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>&, const type&)
      {
      }

      // Insertion operators for simple_type.
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>&, const simple_type&)
      {
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>&, const simple_type&)
      {
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>&, const simple_type&)
      {
      }

      // Insertion operators for sequence.
      //
      template <typename C, typename X, bool fund>
      void
      operator<< (xml::dom::element<C>& e, const sequence<X, fund>& v)
      {
        std::basic_ostringstream<C> os;
        xml::dom::list_stream<C> ls (os, e);

        ls << v;

        e.value (os.str ());
      }

      template <typename C, typename X, bool fund>
      void
      operator<< (xml::dom::attribute<C>& a, const sequence<X, fund>& v)
      {
        std::basic_ostringstream<C> os;
        xml::dom::list_stream<C> ls (os, a.element ());

        ls << v;

        a.value (os.str ());
      }

      template <typename C, typename X, bool fund>
      void
      operator<< (xml::dom::list_stream<C>& ls, const sequence<X, fund>& v)
      {
        for (typename sequence<X, fund>::const_iterator
               b (v.begin ()), e (v.end ()), i (b); i != e; ++i)
        {
          if (i != b)
            ls.os_ << C (' ');

          ls << *i;
        }
      }


      // Insertion operators for fundamental_base.
      //
      template <typename C, typename X>
      void
      operator<< (xml::dom::element<C>& e, const fundamental_base<X>& x)
      {
        const X& r (x);
        e << r;
      }

      template <typename C, typename X>
      void
      operator<< (xml::dom::attribute<C>& a, const fundamental_base<X>& x)
      {
        const X& r (x);
        a << r;
      }

      template <typename C, typename X>
      void
      operator<< (xml::dom::list_stream<C>& ls, const fundamental_base<X>& x)
      {
        const X& r (x);
        ls << r;
      }


      // Insertion operators for built-in types.
      //


      namespace bits
      {
        template <typename C, typename X>
        void
        insert (xml::dom::element<C>& e, const X& x)
        {
          std::basic_ostringstream<C> os;
          os << x;
          e.value (os.str ());
        }

        template <typename C, typename X>
        void
        insert (xml::dom::attribute<C>& a, const X& x)
        {
          std::basic_ostringstream<C> os;
          os << x;
          a.value (os.str ());
        }
      }


      // string
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const string<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const string<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const string<C>& x)
      {
        ls.os_ << x;
      }


      // normalized_string
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e,
                  const normalized_string<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a,
                  const normalized_string<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls,
                  const normalized_string<C>& x)
      {
        ls.os_ << x;
      }


      // token
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const token<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const token<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const token<C>& x)
      {
        ls.os_ << x;
      }


      // nmtoken
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const nmtoken<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const nmtoken<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const nmtoken<C>& x)
      {
        ls.os_ << x;
      }


      // nmtokens
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const nmtokens<C>& v)
      {
        const sequence<nmtoken<C> >& r (v);
        e << r;
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const nmtokens<C>& v)
      {
        const sequence<nmtoken<C> >& r (v);
        a << r;
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const nmtokens<C>& v)
      {
        const sequence<nmtoken<C> >& r (v);
        ls << r;
      }


      // name
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const name<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const name<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const name<C>& x)
      {
        ls.os_ << x;
      }


      // ncname
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const ncname<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const ncname<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const ncname<C>& x)
      {
        ls.os_ << x;
      }


      // language
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const language<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const language<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const language<C>& x)
      {
        ls.os_ << x;
      }


      // id
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const id<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const id<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const id<C>& x)
      {
        ls.os_ << x;
      }


      // idref
      //
      template <typename C, typename X>
      inline void
      operator<< (xml::dom::element<C>& e, const idref<X, C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C, typename X>
      inline void
      operator<< (xml::dom::attribute<C>& a, const idref<X, C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C, typename X>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const idref<X, C>& x)
      {
        ls.os_ << x;
      }


      // idrefs
      //
      template <typename C, typename X>
      inline void
      operator<< (xml::dom::element<C>& e, const idrefs<X, C>& v)
      {
        const sequence<idref<X, C> >& r (v);
        e << r;
      }

      template <typename C, typename X>
      inline void
      operator<< (xml::dom::attribute<C>& a, const idrefs<X, C>& v)
      {
        const sequence<idref<X, C> >& r (v);
        a << r;
      }

      template <typename C, typename X>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const idrefs<X, C>& v)
      {
        const sequence<idref<X, C> >& r (v);
        ls << r;
      }


      // uri
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const uri<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const uri<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const uri<C>& x)
      {
        ls.os_ << x;
      }


      // qname
      //
      template <typename C>
      void
      operator<< (xml::dom::element<C>& e, const qname<C>& x)
      {
        std::basic_ostringstream<C> os;
        std::basic_string<C> p (xml::dom::prefix<C> (x.namespace_ (), e));

        if (!p.empty ())
          os << p << C (':');

        os << x.name ();

        e.value (os.str ());
      }

      template <typename C>
      void
      operator<< (xml::dom::attribute<C>& a, const qname<C>& x)
      {
        std::basic_ostringstream<C> os;
        std::basic_string<C> p (
          xml::dom::prefix<C> (x.namespace_ (), a.element ()));

        if (!p.empty ())
          os << p << C (':');

        os << x.name ();

        a.value (os.str ());
      }

      template <typename C, typename X>
      void
      operator<< (xml::dom::list_stream<C>& ls, const qname<C>& x)
      {
        std::basic_string<C> p (
          xml::dom::prefix<C> (x.namespace_ (), ls.parent));

        if (!p.empty ())
          ls.os_ << p << C (':');

        ls.os_ << x.name ();
      }


      // base64_binary
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const base64_binary<C>& x)
      {
        e.value (x.encode ());
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const base64_binary<C>& x)
      {
        a.value (x.encode ());
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const base64_binary<C>& x)
      {
        ls.os_ << x.encode ();
      }


      // hex_binary
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const hex_binary<C>& x)
      {
        e.value (x.encode ());
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const hex_binary<C>& x)
      {
        a.value (x.encode ());
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const hex_binary<C>& x)
      {
        ls.os_ << x.encode ();
      }


      // date
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const date<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const date<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const date<C>& x)
      {
        ls.os_ << x;
      }


      // date_time
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const date_time<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const date_time<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const date_time<C>& x)
      {
        ls.os_ << x;
      }


      // duration
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const duration<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const duration<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const duration<C>& x)
      {
        ls.os_ << x;
      }


      // day
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const day<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const day<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const day<C>& x)
      {
        ls.os_ << x;
      }


      // month
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const month<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const month<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const month<C>& x)
      {
        ls.os_ << x;
      }


      // month_day
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const month_day<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const month_day<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const month_day<C>& x)
      {
        ls.os_ << x;
      }


      // year
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const year<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const year<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const year<C>& x)
      {
        ls.os_ << x;
      }


      // year_month
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const year_month<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const year_month<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const year_month<C>& x)
      {
        ls.os_ << x;
      }


      // time
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const time<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const time<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const time<C>& x)
      {
        ls.os_ << x;
      }


      // entity
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const entity<C>& x)
      {
        bits::insert (e, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const entity<C>& x)
      {
        bits::insert (a, x);
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const entity<C>& x)
      {
        ls.os_ << x;
      }


      // entities
      //
      template <typename C>
      inline void
      operator<< (xml::dom::element<C>& e, const entities<C>& v)
      {
        const sequence<entity<C> >& r (v);
        e << r;
      }

      template <typename C>
      inline void
      operator<< (xml::dom::attribute<C>& a, const entities<C>& v)
      {
        const sequence<entity<C> >& r (v);
        a << r;
      }

      template <typename C>
      inline void
      operator<< (xml::dom::list_stream<C>& ls, const entities<C>& v)
      {
        const sequence<entity<C> >& r (v);
        ls << r;
      }
    }
  }
}

#endif  // XSD_CXX_TREE_SERIALIZATION_HXX

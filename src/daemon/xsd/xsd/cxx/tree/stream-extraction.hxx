// file      : xsd/cxx/tree/stream-extraction.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_STREAM_EXTRACTION_HXX
#define XSD_CXX_TREE_STREAM_EXTRACTION_HXX

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/types.hxx>

#include <xsd/cxx/tree/istream.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // type
      //
      template <typename S>
      inline _type::
      _type (istream<S>&, flags, type* container)
          : container_ (container)
      {
      }


      // simple_type
      //
      template <typename S>
      inline simple_type::
      simple_type (istream<S>& s, flags f, type* container)
          : type (s, f, container)
      {
      }


      // fundamental_base
      //
      template <typename X>
      template <typename S>
      inline fundamental_base<X>::
      fundamental_base (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        X& r (*this);
        s >> r;
      }


      // optional
      //
      template <typename X>
      template <typename S>
      optional<X, false>::
      optional (istream<S>& s, flags f, type* container)
          : x_ (0), flags_ (f), container_ (container)
      {
        bool p;
        s >> p;

        if (p)
          x_ = new X (s, f, container);
      }

      template <typename X>
      template <typename S>
      optional<X, true>::
      optional (istream<S>& s, flags, type*)
          : present_ (false)
      {
        bool p;
        s >> p;

        if (p)
        {
          X x;
          s >> x;
          set (x);
        }
      }


      // sequence
      //
      template <typename X>
      template <typename S>
      sequence<X, false>::
      sequence (istream<S>& s, flags f, type* container)
          : sequence_common (f, container)
      {
        std::size_t size;
        istream_common::as_size<std::size_t> as_size (size);
        s >> as_size;

        if (size > 0)
        {
          this->reserve (size);

          while (size--)
          {
            ptr p (new X (s, f, container));
            v_.push_back (p);
          }
        }
      }

      template <typename X>
      template <typename S>
      sequence<X, true>::
      sequence (istream<S>& s, flags, type*)
      {
        std::size_t size;
        s >> istream_common::as_size<std::size_t> (size);

        if (size > 0)
        {
          this->reserve (size);

          while (size--)
          {
            X x;
            s >> x;
            push_back (x);
          }
        }
      }

      template <typename X>
      template <typename S>
      one<X, false>::
      one (istream<S>& s, flags f, type* container)
          : x_ (0), flags_ (f), container_ (container)
      {
        x_ = new X (s, f, container);
      }

      template <typename X>
      template <typename S>
      one<X, true>::
      one (istream<S>& s, flags, type*)
          : present_ (false)
      {
        s >> x_;
        present_ = true;
      }


      // Extraction operators for built-in types.
      //


      // string
      //
      template <typename C>
      template <typename S>
      inline string<C>::
      string (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // normalized_string
      //
      template <typename C>
      template <typename S>
      inline normalized_string<C>::
      normalized_string (istream<S>& s, flags f, type* container)
          : string<C> (s, f, container)
      {
      }


      // token
      //
      template <typename C>
      template <typename S>
      inline token<C>::
      token (istream<S>& s, flags f, type* container)
          : normalized_string<C> (s, f, container)
      {
      }


      // nmtoken
      //
      template <typename C>
      template <typename S>
      inline nmtoken<C>::
      nmtoken (istream<S>& s, flags f, type* container)
          : token<C> (s, f, container)
      {
      }


      // nmtokens
      //
      template <typename C>
      template <typename S>
      inline nmtokens<C>::
      nmtokens (istream<S>& s, flags f, type* container)
          : simple_type (f, container), base_type (s, f, container)
      {
      }


      // name
      //
      template <typename C>
      template <typename S>
      inline name<C>::
      name (istream<S>& s, flags f, type* container)
          : token<C> (s, f, container)
      {
      }


      // ncname
      //
      template <typename C>
      template <typename S>
      inline ncname<C>::
      ncname (istream<S>& s, flags f, type* container)
          : name<C> (s, f, container)
      {
      }


      // language
      //
      template <typename C>
      template <typename S>
      inline language<C>::
      language (istream<S>& s, flags f, type* container)
          : token<C> (s, f, container)
      {
      }


      // id
      //
      template <typename C>
      template <typename S>
      inline id<C>::
      id (istream<S>& s, flags f, type* container)
          : ncname<C> (s, f, container), identity_ (*this)
      {
        register_id ();
      }


      // idref
      //
      template <typename X, typename C>
      template <typename S>
      inline idref<X, C>::
      idref (istream<S>& s, flags f, type* container)
          : ncname<C> (s, f, container), identity_ (*this)
      {
      }


      // idrefs
      //
      template <typename X, typename C>
      template <typename S>
      inline idrefs<X, C>::
      idrefs (istream<S>& s, flags f, type* container)
          : simple_type (f, container), base_type (s, f, container)
      {
      }


      // uri
      //
      template <typename C>
      template <typename S>
      inline uri<C>::
      uri (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // qname
      //
      template <typename C>
      template <typename S>
      inline qname<C>::
      qname (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container), ns_ (s), name_ (s)
      {
      }


      // base64_binary
      //
      template <typename C>
      template <typename S>
      inline base64_binary<C>::
      base64_binary (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        buffer<C>& r (*this);
        s >> r;
      }


      // hex_binary
      //
      template <typename C>
      template <typename S>
      inline hex_binary<C>::
      hex_binary (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        buffer<C>& r (*this);
        s >> r;
      }


      // date
      //
      template <typename C>
      template <typename S>
      inline date<C>::
      date (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // date_time
      //
      template <typename C>
      template <typename S>
      inline date_time<C>::
      date_time (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // duration
      //
      template <typename C>
      template <typename S>
      inline duration<C>::
      duration (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // day
      //
      template <typename C>
      template <typename S>
      inline day<C>::
      day (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // month
      //
      template <typename C>
      template <typename S>
      inline month<C>::
      month (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // month_day
      //
      template <typename C>
      template <typename S>
      inline month_day<C>::
      month_day (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // year
      //
      template <typename C>
      template <typename S>
      inline year<C>::
      year (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // year_month
      //
      template <typename C>
      template <typename S>
      inline year_month<C>::
      year_month (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // time
      //
      template <typename C>
      template <typename S>
      inline time<C>::
      time (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container)
      {
        std::basic_string<C>& r (*this);
        s >> r;
      }


      // entity
      //
      template <typename C>
      template <typename S>
      inline entity<C>::
      entity (istream<S>& s, flags f, type* container)
          : ncname<C> (s, f, container)
      {
      }


      // entities
      //
      template <typename C>
      template <typename S>
      inline entities<C>::
      entities (istream<S>& s, flags f, type* container)
          : simple_type (s, f, container), base_type (s, f, container)
      {
      }
    }
  }
}

#endif  // XSD_CXX_TREE_STREAM_EXTRACTION_HXX

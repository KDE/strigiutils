// file      : xsd/cxx/xml/string.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_XML_STRING_HXX
#define XSD_CXX_XML_STRING_HXX

#include <string>

#include <xercesc/util/XMLString.hpp>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      template <typename C>
      std::basic_string<C>
      transcode (const XMLCh* s, std::size_t length);

      template <typename C>
      std::basic_string<C>
      transcode (const XMLCh* s)
      {
        return transcode<C> (s, 0);
      }

      template <typename C>
      XMLCh*
      transcode (const std::basic_string<C>& s);

      //
      //
      class string
      {
      public :
        template <typename C>
        string (const std::basic_string<C>& s)
            : s_ (transcode<C> (s))
        {
        }

        template <typename C>
        string (const C* s)
	    // For VC7.1 wchar_t and XMLCh are the same type.
	    //
            : s_ (transcode<C> (std::basic_string<C> (s)))
        {
        }

        ~string ()
        {
          xercesc::XMLString::release (&s_);
        }

        const XMLCh*
        c_str () const
        {
          return s_;
        }

      private:
        string (const string&);

        string&
        operator= (const string&);

      private:
        XMLCh* s_;
      };
    }
  }
}

#endif  // XSD_CXX_XML_STRING_HXX

#include <xsd/cxx/xml/string.ixx>

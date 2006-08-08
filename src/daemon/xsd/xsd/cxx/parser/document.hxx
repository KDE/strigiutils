// file      : xsd/cxx/parser/document.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_PARSER_DOCUMENT_HXX
#define XSD_CXX_PARSER_DOCUMENT_HXX

#include <string>
#include <cstdlib> // std::size_t

#include <xsd/cxx/parser/elements.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      // If you want to use a different underlying XML parser, all you
      // need to do is to route events to this interface and then use
      // the document type below.
      //
      template <typename C>
      struct event_consumer
      {
        virtual
        ~event_consumer ();

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
      };


      //
      //
      template <typename C>
      struct document: event_consumer<C>
      {
        typedef std::basic_string<C> string;

        document (parser_base<C>& root,
                  const string& ns,
                  const string& name);

      public:
        virtual void
        _start_element (const string&, const string&);

        virtual void
        _end_element (const string&, const string&);

        virtual void
        _attribute (const string&, const string&, const string&);

        virtual void
        _characters (const string&);

      private:
        parser_base<C>& root_;
        string name_;
        string ns_;
        std::size_t depth_;
      };
    }
  }
}

#include <xsd/cxx/parser/document.txx>

#endif  // XSD_CXX_PARSER_DOCUMENT_HXX

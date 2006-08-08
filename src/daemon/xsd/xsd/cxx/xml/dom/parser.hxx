// file      : xsd/cxx/xml/dom/parser.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_XML_DOM_PARSER_HXX
#define XSD_CXX_XML_DOM_PARSER_HXX

#include <xsd/cxx/xml/dom/elements.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace dom
      {
        template <typename C>
        class parser
        {
        public:
          parser (const element<C>& e);

          bool
          more_elements () const
          {
            return e_->getLength () > ei_;
          }

          element<C>
          next_element ();

          bool
          more_attributes () const
          {
            return a_->getLength () > ai_;
          }

          attribute<C>
          next_attribute ();

        private:
          void
          find_next_element ();

        private:
          parser (const parser&);

          parser&
          operator= (const parser&);

        private:
          const xercesc::DOMNodeList* e_;
          unsigned long ei_; // Index of the next DOMElement.

          const xercesc::DOMNamedNodeMap* a_;
          unsigned long ai_; // Index of the next DOMAttr.
        };
      }
    }
  }
}

#include <xsd/cxx/xml/dom/parser.txx>

#endif // XSD_CXX_XML_DOM_PARSER_HXX

// file      : xsd/cxx/xml/dom/parser.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cassert>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace dom
      {
        template <typename C>
        parser<C>::
        parser (const element<C>& e)
            : e_ (e.dom_element ()->getChildNodes ()), ei_ (0),
              a_ (e.dom_element ()->getAttributes ()), ai_ (0)
        {
          find_next_element ();
        }

        template <typename C>
        element<C> parser<C>::
        next_element ()
        {
          using xercesc::DOMNode;

          DOMNode* n (e_->item (ei_++));
          assert (n->getNodeType () == DOMNode::ELEMENT_NODE);

          find_next_element ();

          return element<C> (static_cast<xercesc::DOMElement*> (n));
        }

        template <typename C>
        attribute<C> parser<C>::
        next_attribute ()
        {
          using xercesc::DOMNode;

          DOMNode* n (a_->item (ai_++));
          assert (n->getNodeType () == DOMNode::ATTRIBUTE_NODE);

          return attribute<C> (static_cast<xercesc::DOMAttr*> (n));
        }

        template <typename C>
        void parser<C>::
        find_next_element ()
        {
          using xercesc::DOMNode;

          for (; ei_ < e_->getLength () &&
                 e_->item (ei_)->getNodeType () != DOMNode::ELEMENT_NODE;
               ++ei_);
        }
      }
    }
  }
}


// file      : xsd/cxx/xml/dom/elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_XML_DOM_ELEMENTS_HXX
#define XSD_CXX_XML_DOM_ELEMENTS_HXX

#include <string>

#include <xsd/cxx/xml/string.hxx>
#include <xsd/cxx/xml/elements.hxx>
#include <xsd/cxx/xml/error-handler.hxx>
#include <xsd/cxx/xml/bits/literals.hxx>

#include <xercesc/dom/DOM.hpp>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace dom
      {
        // Simple auto_ptr version that calls release() instead of delete.
        //

        template <typename X>
        struct remove_c
        {
          typedef X r;
        };

        template <typename X>
        struct remove_c<const X>
        {
          typedef X r;
        };

        template <typename X>
        struct auto_ptr_ref
        {
          X* x_;

          explicit
          auto_ptr_ref (X* x)
              : x_ (x)
          {
          }
        };

        template <typename X>
        struct auto_ptr
        {
          ~auto_ptr ()
          {
            reset ();
          }

          explicit
          auto_ptr (X* x = 0)
              : x_ (x)
          {
          }

          auto_ptr (auto_ptr& y)
              : x_ (y.release ())
          {
          }

          template <typename Y>
          auto_ptr (auto_ptr<Y>& y)
              : x_ (y.release ())
          {
          }

          auto_ptr (auto_ptr_ref<X> r)
              : x_ (r.x_)
          {
          }

          auto_ptr&
          operator= (auto_ptr& y)
          {
            if (x_ != y.x_)
            {
              reset (y.release ());
            }

            return *this;
          }

          template <typename Y>
          auto_ptr&
          operator= (auto_ptr<Y>& y)
          {
            if (x_ != y.x_)
            {
              reset (y.release ());
            }

            return *this;
          }

          auto_ptr&
          operator= (auto_ptr_ref<X> r)
          {
            if (r.x_ != x_)
            {
              reset (r.x_);
            }

            return *this;
          }

          template <typename Y>
          operator auto_ptr_ref<Y> ()
          {
            return auto_ptr_ref<Y> (release ());
          }

          template <typename Y>
          operator auto_ptr<Y> ()
          {
            return auto_ptr<Y> (release ());
          }

        public:
          X&
          operator* () const
          {
            return *x_;
          }

          X*
          operator-> () const
          {
            return x_;
          }

          X*
          get () const
          {
            return x_;
          }

          X*
          release ()
          {
            X* x (x_);
            x_ = 0;
            return x;
          }

          void
          reset (X* x = 0)
          {
            if (x_)
              const_cast<typename remove_c<X>::r*> (x_)->release ();

            x_ = x;
          }

          // Conversion to bool.
          //
          typedef X* (auto_ptr::*boolean_convertible)() const;

          operator boolean_convertible () const throw ()
          {
            return x_ ? &auto_ptr<X>::operator-> : 0;
          }

        private:
          X* x_;
        };


        //
        //
        template <typename C>
        class element;


        template <typename C>
        std::basic_string<C>
        prefix (const std::basic_string<C>& ns, const element<C>& e);

        using xml::prefix;


        //
        //
        template <typename C>
        class element
        {
          typedef std::basic_string<C> string_type;

        public:
          element (const xercesc::DOMElement*);

          element (xercesc::DOMElement*);

          element (const string_type& name, element& parent);

          element (const string_type& name,
                   const string_type& ns,
                   element& parent);

        public:
          string_type
          name () const
          {
            return name_;
          }

          string_type
          namespace_ () const
          {
            return namespace__;
          }

        public:
          string_type
          value () const;

          void
          value (const string_type&);

        public:
          string_type
          operator[] (const string_type& name) const;

          string_type
          attribute (const string_type& name, const string_type& ns) const;

        public:
          const xercesc::DOMElement*
          dom_element () const
          {
            return ce_;
          }

          xercesc::DOMElement*
          dom_element ()
          {
            return e_;
          }

        private:
          xercesc::DOMElement* e_;
          const xercesc::DOMElement* ce_;

          string_type name_;
          string_type namespace__;
        };


        //
        //
        template <typename C>
        class attribute
        {
          typedef std::basic_string<C> string_type;

        public:
          attribute (const xercesc::DOMAttr*);

          attribute (xercesc::DOMAttr*);

          attribute (const string_type& name,
                     element<C>& parent,
                     const string_type& value = string_type ());

          attribute (const string_type& name,
                     const string_type& ns,
                     element<C>& parent,
                     const string_type& value = string_type ());

          string_type
          name () const
          {
            return name_;
          }

          string_type
          namespace_ () const
          {
            return namespace__;
          }

          string_type
          value () const
          {
            return value_;
          }

          void
          value (const string_type& v)
          {
            value_ = v;
            a_->setValue (string (v).c_str ());
          }

          dom::element<C>
          element () const;

          dom::element<C>
          element ();

        public:
          const xercesc::DOMAttr*
          dom_attribute () const
          {
            return ca_;
          }

          xercesc::DOMAttr*
          dom_attribute ()
          {
            return a_;
          }

        private:
          xercesc::DOMAttr* a_;
          const xercesc::DOMAttr* ca_;

          string_type name_;
          string_type namespace__;
          string_type value_;
        };

        struct no_mapping {};

        template <typename C>
        std::basic_string<C>
        ns_name (const element<C>& e, const std::basic_string<C>& n)
        {
          std::basic_string<C> p (prefix (n));

          // 'xml' prefix requires special handling and Xerces folks refuse
          // to handle this in DOM so I have to do it myself.
          //
          if (p == xml::bits::xml_prefix<C> ())
            return xml::bits::xml_namespace<C> ();

          const XMLCh* xns (e.dom_element ()->lookupNamespaceURI (
                              p.empty () ? 0 : string (p).c_str ()));

          if (xns == 0)
            throw no_mapping ();

          std::basic_string<C> ns (
            xns ? transcode<C> (xns) : std::basic_string<C> ());

          return ns;

        }


        template <typename C>
        std::basic_string<C>
        fq_name (const element<C>& e, const std::basic_string<C>& n)
        {
          std::basic_string<C> ns (ns_name (e, n));
          std::basic_string<C> un (uq_name (n));

          return ns.empty () ? un : (ns + C ('#') + un);
        }

        class no_prefix {};

        template <typename C>
        std::basic_string<C>
        prefix (const std::basic_string<C>& ns, const element<C>& e)
        {
          string xns (ns);

          const XMLCh* p (
            e.dom_element ()->lookupNamespacePrefix (xns.c_str (), false));

          if (p == 0)
          {
            bool r (e.dom_element ()->isDefaultNamespace (xns.c_str ()));

            if (r)
              return std::basic_string<C> ();
            else
            {
              // 'xml' prefix requires special handling and Xerces folks
              // refuse to handle this in DOM so I have to do it myself.
              //
              if (ns == xml::bits::xml_namespace<C> ())
                return xml::bits::xml_prefix<C> ();

              throw no_prefix ();
            }
          }

          return transcode<C> (p);
        }


        // Parsing flags.
        //
        const unsigned long dont_validate = 0x00000200UL;

        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        parse (const xercesc::DOMInputSource&,
               error_handler<C>&,
               const properties<C>&,
               unsigned long flags);

        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        parse (const xercesc::DOMInputSource&,
               xercesc::DOMErrorHandler&,
               const properties<C>&,
               unsigned long flags);

        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        parse (const std::basic_string<C>& uri,
               error_handler<C>&,
               const properties<C>&,
               unsigned long flags);

        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        parse (const std::basic_string<C>& uri,
               xercesc::DOMErrorHandler&,
               const properties<C>&,
               unsigned long flags);
      }
    }
  }
}

#include <xsd/cxx/xml/dom/elements.txx>

#endif // XSD_CXX_XML_DOM_ELEMENTS_HXX

// file      : xsd/cxx/xml/dom/elements.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <xercesc/util/XMLUniDefs.hpp> // chLatin_L, etc

#include <xsd/cxx/xml/dom/bits/error-handler-proxy.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace dom
      {
        // element
        //
        template <typename C>
        element<C>::
        element (const xercesc::DOMElement* e)
            : e_ (0),
              ce_ (e),
              name_ (transcode<C> (e->getLocalName ())),
              namespace__ (transcode<C> (e->getNamespaceURI ()))
        {
        }

        template <typename C>
        element<C>::
        element (xercesc::DOMElement* e)
            : e_ (e),
              ce_ (e),
              name_ (transcode<C> (e->getLocalName ())),
              namespace__ (transcode<C> (e->getNamespaceURI ()))
        {
        }

        template <typename C>
        element<C>::
        element (const string_type& name, element& parent)
            : e_ (0),
              ce_ (0),
              name_ (name)
        {
          xercesc::DOMDocument* doc (
            parent.dom_element ()->getOwnerDocument ());

          e_ = doc->createElement (string (name).c_str ());

          parent.dom_element ()->appendChild (e_);

          ce_ = e_;
        }

        template <typename C>
        element<C>::
        element (const string_type& name,
                 const string_type& ns,
                 element& parent)
            : e_ (0),
              ce_ (0),
              name_ (name),
              namespace__ (ns)
        {
          string_type p (prefix (ns, parent));

          xercesc::DOMDocument* doc (
            parent.dom_element ()->getOwnerDocument ());

          e_ = doc->createElementNS (
            string (ns).c_str (),
            string (p.empty ()
                    ? name
                    : p + string_type (1, ':') + name).c_str ());

          parent.dom_element ()->appendChild (e_);

          ce_ = e_;
        }

        template <typename C>
        typename element<C>::string_type element<C>::
        value () const
        {
          return transcode<C> (dom_element ()->getTextContent ());
        }

        template <typename C>
        void element<C>::
        value (const string_type& v)
        {
          xercesc::DOMText* text (
            dom_element ()->getOwnerDocument ()->createTextNode(
              string (v).c_str ()));

          dom_element ()->appendChild (text);
        }

        template <typename C>
        typename element<C>::string_type element<C>::
        operator[] (const string_type& name) const
        {
          const XMLCh* value (ce_->getAttribute (string (name).c_str ()));
          return transcode<C> (value);
        }

        template <typename C>
        typename element<C>::string_type element<C>::
        attribute (const string_type& name, const string_type& ns) const
        {
          const XMLCh* value (
            ce_->getAttributeNS (string (ns).c_str (),
                                 string (name).c_str ()));
          return transcode<C> (value);
        }


        // attribute
        //

        template <typename C>
        attribute<C>::
        attribute (const xercesc::DOMAttr* a)
            : a_ (0),
              ca_ (a),
              name_ (transcode<C> (a->getLocalName ())),
              namespace__ (transcode<C> (a->getNamespaceURI ())),
              value_ (transcode<C> (a->getValue ()))
        {
        }

        template <typename C>
        attribute<C>::
        attribute (xercesc::DOMAttr* a)
            : a_ (a),
              ca_ (a),
              name_ (transcode<C> (a->getLocalName ())),
              namespace__ (transcode<C> (a->getNamespaceURI ())),
              value_ (transcode<C> (a->getValue ()))
        {
        }

        template <typename C>
        attribute<C>::
        attribute (const string_type& name,
                   dom::element<C>& parent,
                   const string_type& v)
            : a_ (0),
              ca_ (0),
              name_ (name),
              value_ ()
        {
          xercesc::DOMDocument* doc (
            parent.dom_element ()->getOwnerDocument ());

          a_ = doc->createAttribute (string (name).c_str ());

          if (!v.empty ())
            value (v);

          parent.dom_element ()->setAttributeNode (a_);

          ca_ = a_;
        }

        template <typename C>
        attribute<C>::
        attribute (const string_type& name,
                   const string_type& ns,
                   dom::element<C>& parent,
                   const string_type& v)
            : a_ (0),
              ca_ (0),
              name_ (name),
              namespace__ (ns),
              value_ ()
        {
          string_type p (prefix (ns, parent));

          xercesc::DOMDocument* doc (
            parent.dom_element ()->getOwnerDocument ());

          a_ = doc->createAttributeNS (
            string (ns).c_str (),
            string (p.empty ()
                    ? name
                    : p + string_type (1, ':') + name).c_str ());

          if (!v.empty ())
            value (v);

          parent.dom_element ()->setAttributeNodeNS (a_);

          ca_ = a_;
        }

        template <typename C>
        element<C> attribute<C>::
        element () const
        {
          return dom::element<C> (
            static_cast<const xercesc::DOMElement*> (
              ca_->getOwnerElement ()));
        }

        template <typename C>
        element<C> attribute<C>::
        element ()
        {
          return dom::element<C> (a_->getOwnerElement ());
        }


        //
        //

        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        parse (const xercesc::DOMInputSource& is,
               error_handler<C>& eh,
               const properties<C>& prop,
               unsigned long flags)
        {
          bits::error_handler_proxy<C> ehp (eh);
          return xml::dom::parse (is, ehp, prop, flags);
        }

        template <typename C>
        auto_ptr<xercesc::DOMDocument>
        parse (const xercesc::DOMInputSource& is,
               xercesc::DOMErrorHandler& eh,
               const properties<C>& prop,
               unsigned long flags)
        {
          // HP aCC cannot handle using namespace xercesc;
          //
          using xercesc::DOMImplementationRegistry;
          using xercesc::DOMImplementationLS;
          using xercesc::DOMDocument;
          using xercesc::DOMBuilder;
          using xercesc::XMLUni;


          // Instantiate the DOM parser.
          //
          const XMLCh gLS[] = {xercesc::chLatin_L,
                               xercesc::chLatin_S,
                               xercesc::chNull};

          // Get an implementation of the Load-Store (LS) interface.
          //
          DOMImplementationLS* impl (
            static_cast<DOMImplementationLS*>(
              DOMImplementationRegistry::getDOMImplementation(gLS)));

          // Create a DOMBuilder.
          //
          auto_ptr<DOMBuilder> parser (
            impl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0));

          // Discard comment nodes in the document.
          //
          parser->setFeature (XMLUni::fgDOMComments, false);

          // Disable datatype normalization. The XML 1.0 attribute value
          // normalization always occurs though.
          //
          parser->setFeature (XMLUni::fgDOMDatatypeNormalization, true);

          // Do not create EntityReference nodes in the DOM tree. No
          // EntityReference nodes will be created, only the nodes
          // corresponding to their fully expanded substitution text will be
          // created.
          //
          parser->setFeature (XMLUni::fgDOMEntities, false);

          // Perform Namespace processing.
          //
          parser->setFeature (XMLUni::fgDOMNamespaces, true);

          // Do not include ignorable whitespace in the DOM tree.
          //
          parser->setFeature (XMLUni::fgDOMWhitespaceInElementContent, false);

          if (flags & dont_validate)
          {
            parser->setFeature (XMLUni::fgDOMValidation, false);
            parser->setFeature (XMLUni::fgXercesSchema, false);
            parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
            //parser->setFeature (XMLUni::fgXercesDOMHasPSVIInfo, false);
          }
          else
          {
            parser->setFeature (XMLUni::fgDOMValidation, true);
            parser->setFeature (XMLUni::fgXercesSchema, true);
            parser->setFeature (XMLUni::fgXercesSchemaFullChecking, true);
            //parser->setFeature (XMLUni::fgXercesDOMHasPSVIInfo, true);
          }

          // We will release DOM ourselves.
          //
          parser->setFeature (XMLUni::fgXercesUserAdoptsDOMDocument, true);


          // Transfer properies if any.
          //

          if (!prop.schema_location ().empty ())
          {
            xml::string sl (prop.schema_location ());
            const void* v (sl.c_str ());

            parser->setProperty (
              XMLUni::fgXercesSchemaExternalSchemaLocation,
              const_cast<void*> (v));
          }

          if (!prop.no_namespace_schema_location ().empty ())
          {
            xml::string sl (prop.no_namespace_schema_location ());
            const void* v (sl.c_str ());

            parser->setProperty (
              XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
              const_cast<void*> (v));
          }

          // Set error handler.
          //
          bits::error_handler_proxy<C> ehp (eh);
          parser->setErrorHandler (&ehp);

          auto_ptr<DOMDocument> doc (parser->parse (is));

          if (ehp.failed ())
            doc.reset ();

          return doc;
        }

        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        parse (const std::basic_string<C>& uri,
               error_handler<C>& eh,
               const properties<C>& prop,
               unsigned long flags)
        {
          bits::error_handler_proxy<C> ehp (eh);
          return xml::dom::parse (uri, ehp, prop, flags);
        }

        template <typename C>
        auto_ptr<xercesc::DOMDocument>
        parse (const std::basic_string<C>& uri,
               xercesc::DOMErrorHandler& eh,
               const properties<C>& prop,
               unsigned long flags)
        {
          // HP aCC cannot handle using namespace xercesc;
          //
          using xercesc::DOMImplementationRegistry;
          using xercesc::DOMImplementationLS;
          using xercesc::DOMDocument;
          using xercesc::DOMBuilder;
          using xercesc::XMLUni;


          // Instantiate the DOM parser.
          //
          const XMLCh gLS[] = {xercesc::chLatin_L,
                               xercesc::chLatin_S,
                               xercesc::chNull};

          // Get an implementation of the Load-Store (LS) interface.
          //
          DOMImplementationLS* impl (
            static_cast<DOMImplementationLS*>(
              DOMImplementationRegistry::getDOMImplementation(gLS)));

          // Create a DOMBuilder.
          //
          auto_ptr<DOMBuilder> parser (
            impl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0));

          // Discard comment nodes in the document.
          //
          parser->setFeature (XMLUni::fgDOMComments, false);

          // Disable datatype normalization. The XML 1.0 attribute value
          // normalization always occurs though.
          //
          parser->setFeature (XMLUni::fgDOMDatatypeNormalization, true);

          // Do not create EntityReference nodes in the DOM tree. No
          // EntityReference nodes will be created, only the nodes
          // corresponding to their fully expanded substitution text will be
          // created.
          //
          parser->setFeature (XMLUni::fgDOMEntities, false);

          // Perform Namespace processing.
          //
          parser->setFeature (XMLUni::fgDOMNamespaces, true);

          // Do not include ignorable whitespace in the DOM tree.
          //
          parser->setFeature (XMLUni::fgDOMWhitespaceInElementContent, false);

          if (flags & dont_validate)
          {
            parser->setFeature (XMLUni::fgDOMValidation, false);
            parser->setFeature (XMLUni::fgXercesSchema, false);
            parser->setFeature (XMLUni::fgXercesSchemaFullChecking, false);
            //parser->setFeature (XMLUni::fgXercesDOMHasPSVIInfo, false);
          }
          else
          {
            parser->setFeature (XMLUni::fgDOMValidation, true);
            parser->setFeature (XMLUni::fgXercesSchema, true);
            parser->setFeature (XMLUni::fgXercesSchemaFullChecking, true);
            //parser->setFeature (XMLUni::fgXercesDOMHasPSVIInfo, true);
          }

          // We will release DOM ourselves.
          //
          parser->setFeature (XMLUni::fgXercesUserAdoptsDOMDocument, true);

          // Transfer properies if any.
          //

          if (!prop.schema_location ().empty ())
          {
            xml::string sl (prop.schema_location ());
            const void* v (sl.c_str ());

            parser->setProperty (
              XMLUni::fgXercesSchemaExternalSchemaLocation,
              const_cast<void*> (v));
          }

          if (!prop.no_namespace_schema_location ().empty ())
          {
            xml::string sl (prop.no_namespace_schema_location ());
            const void* v (sl.c_str ());

            parser->setProperty (
              XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation,
              const_cast<void*> (v));
          }

          // Set error handler.
          //
          bits::error_handler_proxy<C> ehp (eh);
          parser->setErrorHandler (&ehp);

          auto_ptr<DOMDocument> doc (
            parser->parseURI (string (uri).c_str ()));

          if (ehp.failed ())
            doc.reset ();

          return doc;
        }
      }
    }
  }
}

// file      : xsd/cxx/xml/dom/serialization.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_XML_DOM_SERIALIZATION_HXX
#define XSD_CXX_XML_DOM_SERIALIZATION_HXX

#include <map>
#include <string>
#include <ostream>
#include <sstream>

#include <xsd/cxx/xml/error-handler.hxx>
#include <xsd/cxx/xml/dom/elements.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace xml
    {
      namespace dom
      {
        //
        //
        template <typename C>
        struct namespace_info
        {
          typedef std::basic_string<C> string;

          namespace_info ()
          {
          }

          namespace_info (const string& name_, const string& schema_)
              : name (name_),
                schema (schema_)
          {
          }

          std::basic_string<C> name;
          std::basic_string<C> schema;
        };


        // Map of namespace prefix to namespace_info.
        //
        template <typename C>
        struct namespace_infomap:
          public std::map<std::basic_string<C>, namespace_info<C> >
        {
        };


        // No mapping provided for a namespace.
        //
        template <typename C>
        struct mapping
        {
          mapping (const std::basic_string<C>& name)
              : name_ (name)
          {
          }

          const std::basic_string<C>&
          name () const
          {
            return name_;
          }

        private:
          std::basic_string<C> name_;
        };

        // Serialization flags.
        //
        const unsigned long no_xml_declaration = 0x00010000UL;


        // 'xsi' prefix is already in use and no user-defined mapping has
        //  been provided.
        //
        struct xsi_already_in_use {};


        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        serialize (const std::basic_string<C>& root_element,
                   const std::basic_string<C>& root_element_namespace,
                   const namespace_infomap<C>& map,
                   unsigned long flags);

        // This one helps Sun C++ to overcome its fears.
        //
        template <typename C>
        xml::dom::auto_ptr<xercesc::DOMDocument>
        serialize (const C* root_element,
                   const C* root_element_namespace,
                   const namespace_infomap<C>& map,
                   unsigned long flags)
        {
          return serialize (std::basic_string<C> (root_element),
                            std::basic_string<C> (root_element_namespace),
                            map,
                            flags);
        }

        //
        //
        template <typename C>
        bool
        serialize (xercesc::XMLFormatTarget& target,
                   const xercesc::DOMDocument& doc,
                   const std::basic_string<C>& enconding,
                   error_handler<C>& eh,
                   unsigned long flags);

        template <typename C>
        bool
        serialize (xercesc::XMLFormatTarget& target,
                   const xercesc::DOMDocument& doc,
                   const std::basic_string<C>& enconding,
                   xercesc::DOMErrorHandler& eh,
                   unsigned long flags);

        //
        //
        class ostream_format_target: public xercesc::XMLFormatTarget
        {
        public:
          ostream_format_target (std::ostream& os)
              : os_ (os)
          {
          }


        public:
          // I know, some of those consts are stupid. But that's what
          // Xerces folks put into their interfaces and VC-7.1 thinks
          // there are different signatures if one strips this fluff off.
          //
          virtual void
          writeChars (const XMLByte* const buf,
                      const unsigned int size,
                      xercesc::XMLFormatter* const)
          {
            os_.write (reinterpret_cast<const char*> (buf),
                       static_cast<std::streamsize> (size));
          }


          virtual void
          flush ()
          {
            os_.flush ();
          }

        private:
          std::ostream& os_;
        };


        //
        //
        template <typename C>
        struct list_stream
        {
          list_stream (std::basic_ostringstream<C>& os,
                       const xml::dom::element<C>& parent)
              : os_ (os), parent_ (parent)
          {
          }

          std::basic_ostringstream<C>& os_;
          const xml::dom::element<C> parent_;
        };

        // Serialization of basic types.
        //

        // enumerators
        //
        template <typename C>
        void
        operator<< (element<C>& e, const C* s)
        {
          e.value (s);
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, const C* s)
        {
          a.value (s);
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, const C* s)
        {
          ls.os_ << s;
        }


        // 8-bit
        //
        template <typename C>
        void
        operator<< (element<C>& e, signed char c)
        {
          std::basic_ostringstream<C> os;
          os << static_cast<short> (c);
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, signed char c)
        {
          std::basic_ostringstream<C> os;
          os << static_cast<short> (c);
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, signed char c)
        {
          ls.os_ << static_cast<short> (c);
        }

        template <typename C>
        void
        operator<< (element<C>& e, unsigned char c)
        {
          std::basic_ostringstream<C> os;
          os << static_cast<unsigned short> (c);
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, unsigned char c)
        {
          std::basic_ostringstream<C> os;
          os << static_cast<unsigned short> (c);
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, unsigned char c)
        {
          ls.os_ << static_cast<unsigned short> (c);
        }

        // 16-bit
        //
        template <typename C>
        void
        operator<< (element<C>& e, short s)
        {
          std::basic_ostringstream<C> os;
          os << s;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, short s)
        {
          std::basic_ostringstream<C> os;
          os << s;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, short s)
        {
          ls.os_ << s;
        }

        template <typename C>
        void
        operator<< (element<C>& e, unsigned short s)
        {
          std::basic_ostringstream<C> os;
          os << s;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, unsigned short s)
        {
          std::basic_ostringstream<C> os;
          os << s;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, unsigned short s)
        {
          ls.os_ << s;
        }

        // 32-bit
        //
        template <typename C>
        void
        operator<< (element<C>& e, int i)
        {
          std::basic_ostringstream<C> os;
          os << i;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, int i)
        {
          std::basic_ostringstream<C> os;
          os << i;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, int i)
        {
          ls.os_ << i;
        }

        template <typename C>
        void
        operator<< (element<C>& e, unsigned int i)
        {
          std::basic_ostringstream<C> os;
          os << i;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, unsigned int i)
        {
          std::basic_ostringstream<C> os;
          os << i;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, unsigned int i)
        {
          ls.os_ << i;
        }

        // 64-bit
        //
        template <typename C>
        void
        operator<< (element<C>& e, long long l)
        {
          std::basic_ostringstream<C> os;
          os << l;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, long long l)
        {
          std::basic_ostringstream<C> os;
          os << l;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, long long l)
        {
          ls.os_ << l;
        }

        template <typename C>
        void
        operator<< (element<C>& n, unsigned long long l)
        {
          std::basic_ostringstream<C> os;
          os << l;
          n.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, unsigned long long l)
        {
          std::basic_ostringstream<C> os;
          os << l;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, unsigned long long l)
        {
          ls.os_ << l;
        }


        // Boolean.
        //
        template <typename C>
        void
        operator<< (element<C>& e, bool b)
        {
          std::basic_ostringstream<C> os;
          os << std::boolalpha << b;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, bool b)
        {
          std::basic_ostringstream<C> os;
          os << std::boolalpha << b;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, bool b)
        {
          std::ios_base::fmtflags f (ls.os_.setf (std::ios_base::boolalpha));
          ls.os_ << b;
          ls.os_.setf (f);
        }

        // Floating-point types. Note that if we are serializing in scientific
        // notation, we need to use (precision + 1). That's how libstdc++
        // works.
        //
        template <typename C>
        void
        operator<< (element<C>& e, float f)
        {
          std::basic_ostringstream<C> os;
          os.precision (7);
          os << f;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, float f)
        {
          std::basic_ostringstream<C> os;
          os.precision (7);
          os << f;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, float f)
        {
          std::streamsize p (ls.os_.precision (7));
          ls.os_ << f;
          ls.os_.precision (p);
        }

        template <typename C>
        void
        operator<< (element<C>& e, double d)
        {
          std::basic_ostringstream<C> os;
          os.precision (13);
          os << d;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, double d)
        {
          std::basic_ostringstream<C> os;
          os.precision (13);
          os << d;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, double d)
        {
          std::streamsize p (ls.os_.precision (13));
          ls.os_ << d;
          ls.os_.precision (p);
        }

        template <typename C>
        void
        operator<< (element<C>& e, long double d)
        {
          // xsd:decimal (which we currently map to long double)
          // can not be in scientific notation.
          //
          std::basic_ostringstream<C> os;
          os.precision (24);
          os << std::fixed << d;
          e.value (os.str ());
        }

        template <typename C>
        void
        operator<< (attribute<C>& a, long double d)
        {
          // xsd:decimal (which we currently map to long double)
          // can not be in scientific notation.
          //
          std::basic_ostringstream<C> os;
          os.precision (24);
          os << std::fixed << d;
          a.value (os.str ());
        }

        template <typename C>
        void
        operator<< (list_stream<C>& ls, long double d)
        {
          // xsd:decimal (which we currently map to long double)
          // can not be in scientific notation.
          //
          std::streamsize p (ls.os_.precision (24));
          std::ios_base::fmtflags f (
            ls.os_.setf (std::ios_base::fixed, std::ios_base::floatfield));

          ls.os_ << d;

          ls.os_.setf (f, std::ios_base::floatfield);
          ls.os_.precision (p);
        }
      }
    }
  }
}

#include <xsd/cxx/xml/dom/serialization.txx>

#endif  // XSD_CXX_XML_DOM_SERIALIZATION_HXX

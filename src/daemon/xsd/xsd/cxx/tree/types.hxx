// file      : xsd/cxx/tree/types.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_TREE_TYPES_HXX
#define XSD_CXX_TREE_TYPES_HXX

#include <cstdlib> // std::size_t
#include <string>

#include <xsd/cxx/xml/dom/elements.hxx>

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>

#include <xsd/cxx/tree/buffer.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      // string
      //
      template <typename C>
      class string: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        string ()
        {
        }

        string (const C* s)
            : base_type (s)
        {
        }

        string (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        string (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        string (const primary_type& str)
            : base_type (str)
        {
        }

        string (const primary_type& str,
                std::size_t pos,
                std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        string (istream<S>&, flags = 0, type* container = 0);

      public:
        string (const string& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container),
              base_type (other)
        {
        }

        virtual string*
        _clone (flags f = 0, type* container = 0) const
        {
          return new string (*this, f, container);
        }

      public:
        string (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        string (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        string (const std::basic_string<C>& s,
                const xml::dom::element<C>* e,
                flags f,
                type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        string&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        string&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        string&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //
      };


      // normalized_string: string
      //
      template <typename C>
      class normalized_string: public string<C>
      {
      protected:
        typedef string<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        normalized_string ()
        {
        }

        normalized_string (const C* s)
            : base_type (s)
        {
        }

        normalized_string (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        normalized_string (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        normalized_string (const primary_type& str)
            : base_type (str)
        {
        }

        normalized_string (const primary_type& str,
                           std::size_t pos,
                           std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        normalized_string (istream<S>&, flags = 0, type* container = 0);

      public:
        normalized_string (const normalized_string& other,
                           flags f = 0,
                           type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual normalized_string*
        _clone (flags f = 0, type* container = 0) const
        {
          return new normalized_string (*this, f, container);
        }

      public:
        normalized_string (const xml::dom::element<C>& e,
                           flags f,
                           type* container)
            : base_type (e, f, container)
        {
        }

        normalized_string (const xml::dom::attribute<C>& a,
                           flags f,
                           type* container)
            : base_type (a, f, container)
        {
        }

        normalized_string (const std::basic_string<C>& s,
                           const xml::dom::element<C>* e,
                           flags f,
                           type* container)
            : base_type (s, e, f, container)
        {
        }

      public:
        normalized_string&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        normalized_string&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        normalized_string&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //
      };


      // token: normalized_string
      //
      template <typename C>
      class token: public normalized_string<C>
      {
      protected:
        typedef normalized_string<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        token ()
        {
        }

        token (const C* s)
            : base_type (s)
        {
        }

        token (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        token (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        token (const primary_type& str)
            : base_type (str)
        {
        }

        token (const primary_type& str,
               std::size_t pos,
               std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        token (istream<S>&, flags = 0, type* container = 0);

      public:
        token (const token& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual token*
        _clone (flags f = 0, type* container = 0) const
        {
          return new token (*this, f, container);
        }

      public:
        token (const xml::dom::element<C>& e, flags f, type* container)
            : base_type (e, f, container)
        {
        }

        token (const xml::dom::attribute<C>& a, flags f, type* container)
            : base_type (a, f, container)
        {
        }

        token (const std::basic_string<C>& s,
               const xml::dom::element<C>* e,
               flags f,
               type* container)
            : base_type (s, e, f, container)
        {
        }

      public:
        token&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        token&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        token&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //
      };


      // nmtoken: token
      //
      template <typename C>
      class nmtoken: public token<C>
      {
      protected:
        typedef token<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        nmtoken (const C* s)
            : base_type (s)
        {
        }

        nmtoken (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        nmtoken (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        nmtoken (const primary_type& str)
            : base_type (str)
        {
        }

        nmtoken (const primary_type& str,
                 std::size_t pos,
                 std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        nmtoken (istream<S>&, flags = 0, type* container = 0);

      public:
        nmtoken (const nmtoken& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual nmtoken*
        _clone (flags f = 0, type* container = 0) const
        {
          return new nmtoken (*this, f, container);
        }

      public:
        nmtoken (const xml::dom::element<C>& e, flags f, type* container)
            : base_type (e, f, container)
        {
        }

        nmtoken (const xml::dom::attribute<C>& a, flags f, type* container)
            : base_type (a, f, container)
        {
        }

        nmtoken (const std::basic_string<C>& s,
                 const xml::dom::element<C>* e,
                 flags f,
                 type* container)
            : base_type (s, e, f, container)
        {
        }

      public:
        nmtoken&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        nmtoken&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        nmtoken&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        nmtoken ()
            : base_type ()
        {
        }
      };


      // nmtokens: sequence<nmtoken>
      //
      template <typename C>
      class nmtokens: public simple_type, public sequence<nmtoken<C> >
      {
        typedef sequence<nmtoken<C> > base_type;

      public:
        nmtokens ()
        {
        }

        template <typename S>
        nmtokens (istream<S>&, flags = 0, type* container = 0);

        nmtokens (const nmtokens& other, flags f, type* container)
            : simple_type (other, f, container),
              base_type (other, f, container)
        {
        }

        virtual nmtokens*
        _clone (flags f = 0, type* container = 0) const
        {
          return new nmtokens (*this, f, container);
        }

      public:
        nmtokens (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e, f, container)
        {
        }

        nmtokens (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a, f, container)
        {
        }

        nmtokens (const std::basic_string<C>& s,
                  const xml::dom::element<C>* e,
                  flags f,
                  type* container)
            : simple_type (s, e, f, container), base_type (s, e, f, container)
        {
        }
      };


      // name: token
      //
      template <typename C>
      class name: public token<C>
      {
      protected:
        typedef token<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        name (const C* s)
            : base_type (s)
        {
        }

        name (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        name (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        name (const primary_type& str)
            : base_type (str)
        {
        }

        name (const primary_type& str,
              std::size_t pos,
              std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        name (istream<S>&, flags = 0, type* container = 0);

      public:
        name (const name& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual name*
        _clone (flags f = 0, type* container = 0) const
        {
          return new name (*this, f, container);
        }

      public:
        name (const xml::dom::element<C>& e, flags f, type* container)
            : base_type (e, f, container)
        {
        }

        name (const xml::dom::attribute<C>& a, flags f, type* container)
            : base_type (a, f, container)
        {
        }

        name (const std::basic_string<C>& s,
              const xml::dom::element<C>* e,
              flags f,
              type* container)
            : base_type (s, e, f, container)
        {
        }

      public:
        name&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        name&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        name&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        name ()
            : base_type ()
        {
        }
      };


      // Forward declaration for Sun CC.
      //
      template <typename C>
      class qname;

      // ncname: name
      //
      template <typename C>
      class ncname: public name<C>
      {
      protected:
        typedef name<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        ncname (const C* s)
            : base_type (s)
        {
        }

        ncname (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        ncname (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        ncname (const primary_type& str)
            : base_type (str)
        {
        }

        ncname (const primary_type& str,
                std::size_t pos,
                std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        ncname (istream<S>&, flags = 0, type* container = 0);

      public:
        ncname (const ncname& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual ncname*
        _clone (flags f = 0, type* container = 0) const
        {
          return new ncname (*this, f, container);
        }

      public:
        ncname (const xml::dom::element<C>& e, flags f, type* container)
            : base_type (e, f, container)
        {
        }

        ncname (const xml::dom::attribute<C>& a, flags f, type* container)
            : base_type (a, f, container)
        {
        }

        ncname (const std::basic_string<C>& s,
                const xml::dom::element<C>* e,
                flags f,
                type* container)
            : base_type (s, e, f, container)
        {
        }

      public:
        ncname&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        ncname&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        ncname&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        ncname ()
            : base_type ()
        {
        }

        template <typename>
        friend class qname;
      };


      // language: token
      //
      template <typename C>
      class language: public token<C>
      {
      protected:
        typedef token<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        language (const C* s)
            : base_type (s)
        {
        }

        language (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        language (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        language (const primary_type& str)
            : base_type (str)
        {
        }

        language (const primary_type& str,
                  std::size_t pos,
                  std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        language (istream<S>&, flags = 0, type* container = 0);

      public:
        language (const language& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual language*
        _clone (flags f = 0, type* container = 0) const
        {
          return new language (*this, f, container);
        }

      public:
        language (const xml::dom::element<C>& e, flags f, type* container)
            : base_type (e, f, container)
        {
        }

        language (const xml::dom::attribute<C>& a, flags f, type* container)
            : base_type (a, f, container)
        {
        }

        language (const std::basic_string<C>& s,
                  const xml::dom::element<C>* e,
                  flags f,
                  type* container)
            : base_type (s, e, f, container)
        {
        }

      public:
        language&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        language&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        language&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        language ()
            : base_type ()
        {
        }
      };


      //
      //
      template <typename C>
      struct identity_impl: identity
      {
        identity_impl (const ncname<C>& id)
            : id_ (id)
        {
        }

        virtual bool
        before (const identity& y) const
        {
          return id_ < static_cast<const identity_impl&> (y).id_;
        }

        virtual void
        throw_duplicate_id () const
        {
          throw duplicate_id<C> (id_);
        }

      private:
        const ncname<C>& id_;
      };


      // id: ncname
      //
      template <typename C>
      class id: public ncname<C>
      {
      protected:
        typedef ncname<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        ~id()
        {
          unregister_id ();
        }

      public:
        id (const C* s)
            : base_type (s), identity_ (*this)
        {
          register_id ();
        }

        id (const C* s, std::size_t n)
            : base_type (s, n), identity_ (*this)
        {
          register_id ();
        }

        id (std::size_t n, C c)
            : base_type (n, c), identity_ (*this)
        {
          register_id ();
        }

        id (const primary_type& str)
            : base_type (str), identity_ (*this)
        {
          register_id ();
        }

        id (const primary_type& str,
            std::size_t pos,
            std::size_t n = primary_type::npos)
            : base_type (str, pos, n), identity_ (*this)
        {
          register_id ();
        }

        template <typename S>
        id (istream<S>&, flags = 0, type* container = 0);

      public:
        id (const id& other, flags f = 0, type* container = 0)
            : base_type (other, f, container), identity_ (*this)
        {
          register_id ();
        }

        virtual id*
        _clone (flags f = 0, type* container = 0) const
        {
          return new id (*this, f, container);
        }

      public:
        id (const xml::dom::element<C>& e, flags f, type* container)
            : base_type (e, f, container), identity_ (*this)
        {
          register_id ();
        }

        id (const xml::dom::attribute<C>& a, flags f, type* container)
            : base_type (a, f, container), identity_ (*this)
        {
          register_id ();
        }

        id (const std::basic_string<C>& s,
            const xml::dom::element<C>* e,
            flags f,
            type* container)
            : base_type (s, e, f, container), identity_ (*this)
        {
          register_id ();
        }

      public:
        id&
        operator= (C c)
        {
          unregister_id ();
          base () = c;
          register_id ();

          return *this;
        }

        id&
        operator= (const C* s)
        {
          unregister_id ();
          base () = s;
          register_id ();

          return *this;
        }

        id&
        operator= (const primary_type& str)
        {
          unregister_id ();
          base () = str;
          register_id ();

          return *this;
        }

        id&
        operator= (const id& str)
        {
          unregister_id ();
          base () = str;
          register_id ();

          return *this;
        }

      protected:
        id ()
            : base_type (), identity_ (*this)
        {
          register_id ();
        }

        // It would have been cleaner to mention empty and _container
        // with the using-declaration but HP aCC3 can't handle it in
        // some non-trivial to track down cases. So we are going to use
        // the old-n-ugly this-> techniques.
        //
      private:
        void
        register_id ()
        {
          if (this->_container () != this && !this->empty ())
          {
            //std::cerr << "registering " << _container ()
            //          << " as '" << *this
            //          << "' on " << _container () << std::endl;

            this->_container ()->_register_id (identity_,
                                               this->_container ());
          }
        }

        void
        unregister_id ()
        {
          if (this->_container () != this && !this->empty ())
          {
            //std::cerr << "un-registering " << _container ()
            //          << " as '" << *this
            //          << "' on " << _container () << std::endl;

            this->_container ()->_unregister_id (identity_);
          }
        }

      private:
        identity_impl<C> identity_;
      };


      // idref: ncname
      //
      template <typename X, typename C>
      class idref: public ncname<C>
      {
      protected:
        typedef ncname<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        typedef X type;

      public:
        idref (const C* s)
            : base_type (s), identity_ (*this)
        {
        }

        idref (const C* s, std::size_t n)
            : base_type (s, n), identity_ (*this)
        {
        }

        idref (std::size_t n, C c)
            : base_type (n, c), identity_ (*this)
        {
        }

        idref (const primary_type& str)
            : base_type (str), identity_ (*this)
        {
        }

        idref (const primary_type& str,
               std::size_t pos,
               std::size_t n = primary_type::npos)
            : base_type (str, pos, n), identity_ (*this)
        {
        }

        template <typename S>
        idref (istream<S>&, flags = 0, type* container = 0);

      public:
        idref (const idref& other, flags f = 0, tree::type* container = 0)
            : base_type (other, f, container), identity_ (*this)
        {
        }

        virtual idref*
        _clone (flags f = 0, tree::type* container = 0) const
        {
          return new idref (*this, f, container);
        }

      public:
        idref (const xml::dom::element<C>& e, flags f, tree::type* container)
            : base_type (e, f, container), identity_ (*this)
        {
        }

        idref (const xml::dom::attribute<C>& a,
               flags f,
               tree::type* container)
            : base_type (a, f , container), identity_ (*this)
        {
        }

        idref (const std::basic_string<C>& s,
               const xml::dom::element<C>* e,
               flags f,
               tree::type* container)
            : base_type (s, e, f, container), identity_ (*this)
        {
        }

      public:
        idref&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        idref&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        idref&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        idref&
        operator= (const idref& str)
        {
          base () = str;
          return *this;
        }

      public:
        const X*
        operator-> () const
        {
          return get ();
        }

        X*
        operator-> ()
        {
          return get ();
        }

        const X&
        operator* () const
        {
          return *(get ());
        }

        X&
        operator* ()
        {
          return *(get ());
        }

        const X*
        get () const
        {
          return dynamic_cast<const X*> (get_ ());
        }

        X*
        get ()
        {
          return dynamic_cast<X*> (get_ ());
        }

        // Conversion to bool.
        //
        typedef void (idref::*bool_convertible)();

        operator bool_convertible () const
        {
          return get_ () ? &idref::true_ : 0;
        }

      protected:
        idref ()
            : base_type (), identity_ (*this)
        {
        }


        // It would have been cleaner to mention empty, _root, etc. with
        // the using-declaration but HP aCC3 can't handle it in some
        // non-trivial to track down cases. So we are going to use the
        // old-n-ugly this-> techniques.
        //
      private:
        const tree::type*
        get_ () const
        {
          if (!this->empty () && this->_container () != this)
          {
            return this->_root ()->_lookup_id (identity_);
          }
          else
            return 0;
        }

        tree::type*
        get_ ()
        {
          if (!this->empty () && this->_container () != this)
          {
            return this->_root ()->_lookup_id (identity_);
          }
          else
            return 0;
        }

      private:
        void
        true_ ()
        {
        }

      private:
        identity_impl<C> identity_;
      };


      // idrefs: sequence<idref>
      //
      template <typename X, typename C>
      class idrefs: public simple_type, public sequence<idref<X, C> >

      {
        typedef sequence<idref<X, C> > base_type;

      public:
        idrefs ()
        {
        }

        template <typename S>
        idrefs (istream<S>&, flags = 0, type* container = 0);

        idrefs (const idrefs& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container),
              base_type (other, f, container)
        {
        }

        virtual idrefs*
        _clone (flags f = 0, type* container = 0) const
        {
          return new idrefs (*this, f, container);
        }

      public:
        idrefs (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e, f, container)
        {
        }

        idrefs (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a, f, container)
        {
        }

        idrefs (const std::basic_string<C>& s,
                const xml::dom::element<C>* e,
                flags f,
                type* container)
            : simple_type (s, e, f, container), base_type (s, e, f, container)
        {
        }
      };


      // uri
      //
      template <typename C>
      class uri: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        uri (const C* s)
            : base_type (s)
        {
        }

        uri (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        uri (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        uri (const primary_type& str)
            : base_type (str)
        {
        }

        uri (const primary_type& str,
             std::size_t pos,
             std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        uri (istream<S>&, flags = 0, type* container = 0);

      public:
        uri (const uri& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual uri*
        _clone (flags f = 0, type* container = 0) const
        {
          return new uri (*this, f, container);
        }

      public:
        uri (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        uri (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        uri (const std::basic_string<C>& s,
             const xml::dom::element<C>* e,
             flags f,
             type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        uri&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        uri&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        uri&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        uri ()
            : base_type ()
        {
        }

        template <typename>
        friend class qname;
      };


      // qname
      //
      template <typename C>
      class qname: public simple_type
      {
      public:
        qname (const uri<C>& ns, const ncname<C>& name)
            : ns_ (ns), name_ (name)
        {
        }

        template <typename S>
        qname (istream<S>&, flags = 0, type* container = 0);

      public:
        qname (const qname& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container),
              ns_ (other.ns_),
              name_ (other.name_)
        {
          // Note that ns_ and name_ have no DOM association.
          //
        }

        virtual qname*
        _clone (flags f = 0, type* container = 0) const
        {
          return new qname (*this, f, container);
        }

      public:
        qname (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container),
              ns_ (xml::dom::ns_name (e, e.value ())),
              name_ (xml::uq_name (e.value ()))
        {
        }

        qname (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container),
              ns_ (xml::dom::ns_name (a.element (), a.value ())),
              name_ (xml::uq_name (a.value ()))
        {
        }

        qname (const std::basic_string<C>& s,
               const xml::dom::element<C>* e,
               flags f,
               type* container)
            : simple_type (s, e, f, container),
              ns_ (resolve (s, e)),
              name_ (xml::uq_name (s))
        {
        }

      public:
        const uri<C>&
        namespace_ () const
        {
          return ns_;
        }

        const ncname<C>&
        name () const
        {
          return name_;
        }

      protected:
        qname ()
            : ns_ (), name_ ()
        {
        }

      private:
        static uri<C>
        resolve (const std::basic_string<C>& s, const xml::dom::element<C>* e)
        {
          if (e)
            return uri<C> (xml::dom::ns_name (*e, s));
          else
            throw no_prefix_mapping<C> (xml::prefix (s));
        }

      private:
        uri<C> ns_;
        ncname<C> name_;
      };


      // base64_binary
      //
      template <typename C>
      class base64_binary: public simple_type, public buffer<C>
      {
      public:
        typedef typename buffer<C>::size_t size_t;

      public:
        base64_binary (size_t size = 0);
        base64_binary (size_t size, size_t capacity);
        base64_binary (const void* data, size_t size);
        base64_binary (const void* data, size_t size, size_t capacity);

        // If the assume_ownership argument is true, the buffer will
        // assume the ownership of data and will release the memory
        // by calling operator delete ().
        //
        base64_binary (void* data,
                       size_t size,
                       size_t capacity,
                       bool assume_ownership);

        template <typename S>
        base64_binary (istream<S>&, flags = 0, type* container = 0);

      public:
        base64_binary (const base64_binary& other,
                       flags f = 0,
                       type* container = 0)
            : simple_type (other, f, container), buffer<C> (other)
        {
        }

        virtual base64_binary*
        _clone (flags f = 0, type* container = 0) const
        {
          return new base64_binary (*this, f, container);
        }

      public:
        base64_binary (const xml::dom::element<C>& e,
                       flags f,
                       type* container)
            : simple_type (e, f, container)
        {
          decode (e.dom_element ()->getTextContent ());
        }

        base64_binary (const xml::dom::attribute<C>& a,
                       flags f,
                       type* container)
            : simple_type (a, f, container)
        {
          decode (a.dom_attribute ()->getValue ());
        }

        base64_binary (const std::basic_string<C>& s,
                       const xml::dom::element<C>* e,
                       flags f,
                       type* container)
            : simple_type (s, e, f, container)
        {
          decode (xml::string (s).c_str ());
        }

      public:
        std::basic_string<C>
        encode () const;

      public:
        // Implicit copy assignment operator.
        //

      private:
        void
        decode (const XMLCh*);
      };


      // hex_binary
      //
      template <typename C>
      class hex_binary: public simple_type, public buffer<C>
      {
      public:
        typedef typename buffer<C>::size_t size_t;

      public:
        hex_binary (size_t size = 0);
        hex_binary (size_t size, size_t capacity);
        hex_binary (const void* data, size_t size);
        hex_binary (const void* data, size_t size, size_t capacity);

        // If the assume_ownership argument is true, the buffer will
        // assume the ownership of data and will release the memory
        // by calling operator delete ().
        //
        hex_binary (void* data,
                    size_t size,
                    size_t capacity,
                    bool assume_ownership);

        template <typename S>
        hex_binary (istream<S>&, flags = 0, type* container = 0);

      public:
        hex_binary (const hex_binary& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), buffer<C> (other)
        {
        }

        virtual hex_binary*
        _clone (flags f = 0, type* container = 0) const
        {
          return new hex_binary (*this, f, container);
        }

      public:
        hex_binary (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container)
        {
          decode (e.dom_element ()->getTextContent ());
        }

        hex_binary (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container)
        {
          decode (a.dom_attribute ()->getValue ());
        }

        hex_binary (const std::basic_string<C>& s,
                    const xml::dom::element<C>* e,
                    flags f,
                    type* container)
            : simple_type (s, e, f, container)
        {
          decode (xml::string (s).c_str ());
        }

      public:
        std::basic_string<C>
        encode () const;

      public:
        // Implicit copy assignment operator.
        //

      private:
        void
        decode (const XMLCh*);
      };


      // date
      //
      template <typename C>
      class date: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        date (const C* s)
            : base_type (s)
        {
        }

        date (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        date (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        date (const primary_type& str)
            : base_type (str)
        {
        }

        date (const primary_type& str,
              std::size_t pos,
              std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        date (istream<S>&, flags = 0, type* container = 0);

      public:
        date (const date& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual date*
        _clone (flags f = 0, type* container = 0) const
        {
          return new date (*this, f, container);
        }

      public:
        date (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        date (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        date (const std::basic_string<C>& s,
              const xml::dom::element<C>* e,
              flags f,
              type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        date&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        date&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        date&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        date ()
            : base_type ()
        {
        }
      };


      // date_time
      //
      template <typename C>
      class date_time: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        date_time (const C* s)
            : base_type (s)
        {
        }

        date_time (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        date_time (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        date_time (const primary_type& str)
            : base_type (str)
        {
        }

        date_time (const primary_type& str,
                   std::size_t pos,
                   std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        date_time (istream<S>&, flags = 0, type* container = 0);

      public:
        date_time (const date_time& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual date_time*
        _clone (flags f = 0, type* container = 0) const
        {
          return new date_time (*this, f, container);
        }

      public:
        date_time (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        date_time (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        date_time (const std::basic_string<C>& s,
                   const xml::dom::element<C>* e,
                   flags f,
                   type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        date_time&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        date_time&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        date_time&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        date_time ()
            : base_type ()
        {
        }
      };


      // duration
      //
      template <typename C>
      class duration: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        duration (const C* s)
            : base_type (s)
        {
        }

        duration (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        duration (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        duration (const primary_type& str)
            : base_type (str)
        {
        }

        duration (const primary_type& str,
                  std::size_t pos,
                  std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        duration (istream<S>&, flags = 0, type* container = 0);

      public:
        duration (const duration& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual duration*
        _clone (flags f = 0, type* container = 0) const
        {
          return new duration (*this, f, container);
        }

      public:
        duration (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        duration (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        duration (const std::basic_string<C>& s,
                  const xml::dom::element<C>* e,
                  flags f,
                  type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        duration&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        duration&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        duration&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        duration ()
            : base_type ()
        {
        }
      };


      // day
      //
      template <typename C>
      class day: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        day (const C* s)
            : base_type (s)
        {
        }

        day (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        day (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        day (const primary_type& str)
            : base_type (str)
        {
        }

        day (const primary_type& str,
             std::size_t pos,
             std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        day (istream<S>&, flags = 0, type* container = 0);

      public:
        day (const day& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual day*
        _clone (flags f = 0, type* container = 0) const
        {
          return new day (*this, f, container);
        }

      public:
        day (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        day (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        day (const std::basic_string<C>& s,
             const xml::dom::element<C>* e,
             flags f,
             type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        day&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        day&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        day&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        day ()
            : base_type ()
        {
        }
      };


      // month
      //
      template <typename C>
      class month: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        month (const C* s)
            : base_type (s)
        {
        }

        month (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        month (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        month (const primary_type& str)
            : base_type (str)
        {
        }

        month (const primary_type& str,
               std::size_t pos,
               std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        month (istream<S>&, flags = 0, type* container = 0);

      public:
        month (const month& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual month*
        _clone (flags f = 0, type* container = 0) const
        {
          return new month (*this, f, container);
        }

      public:
        month (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        month (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        month (const std::basic_string<C>& s,
               const xml::dom::element<C>* e,
               flags f,
               type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        month&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        month&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        month&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        month ()
            : base_type ()
        {
        }
      };


      // month_day
      //
      template <typename C>
      class month_day: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        month_day (const C* s)
            : base_type (s)
        {
        }

        month_day (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        month_day (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        month_day (const primary_type& str)
            : base_type (str)
        {
        }

        month_day (const primary_type& str,
                   std::size_t pos,
                   std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        month_day (istream<S>&, flags = 0, type* container = 0);

      public:
        month_day (const month_day& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual month_day*
        _clone (flags f = 0, type* container = 0) const
        {
          return new month_day (*this, f, container);
        }

      public:
        month_day (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        month_day (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        month_day (const std::basic_string<C>& s,
                   const xml::dom::element<C>* e,
                   flags f,
                   type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        month_day&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        month_day&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        month_day&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        month_day ()
            : base_type ()
        {
        }
      };


      // year
      //
      template <typename C>
      class year: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        year (const C* s)
            : base_type (s)
        {
        }

        year (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        year (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        year (const primary_type& str)
            : base_type (str)
        {
        }

        year (const primary_type& str,
              std::size_t pos,
              std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        year (istream<S>&, flags = 0, type* container = 0);

      public:
        year (const year& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual year*
        _clone (flags f = 0, type* container = 0) const
        {
          return new year (*this, f, container);
        }

      public:
        year (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        year (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        year (const std::basic_string<C>& s,
              const xml::dom::element<C>* e,
              flags f,
              type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        year&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        year&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        year&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        year ()
            : base_type ()
        {
        }
      };


      // year_month
      //
      template <typename C>
      class year_month: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        year_month (const C* s)
            : base_type (s)
        {
        }

        year_month (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        year_month (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        year_month (const primary_type& str)
            : base_type (str)
        {
        }

        year_month (const primary_type& str,
                    std::size_t pos,
                    std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        year_month (istream<S>&, flags = 0, type* container = 0);

      public:
        year_month (const year_month& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual year_month*
        _clone (flags f = 0, type* container = 0) const
        {
          return new year_month (*this, f, container);
        }

      public:
        year_month (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        year_month (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        year_month (const std::basic_string<C>& s,
                    const xml::dom::element<C>* e,
                    flags f,
                    type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        year_month&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        year_month&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        year_month&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        year_month ()
            : base_type ()
        {
        }
      };


      // time
      //
      template <typename C>
      class time: public simple_type, public std::basic_string<C>
      {
      protected:
        typedef std::basic_string<C> primary_type;
        typedef std::basic_string<C> base_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        time (const C* s)
            : base_type (s)
        {
        }

        time (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        time (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        time (const primary_type& str)
            : base_type (str)
        {
        }

        time (const primary_type& str,
              std::size_t pos,
              std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        time (istream<S>&, flags = 0, type* container = 0);

      public:
        time (const time& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container), base_type (other)
        {
        }

        virtual time*
        _clone (flags f = 0, type* container = 0) const
        {
          return new time (*this, f, container);
        }

      public:
        time (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e.value ())
        {
        }

        time (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a.value ())
        {
        }

        time (const std::basic_string<C>& s,
              const xml::dom::element<C>* e,
              flags f,
              type* container)
            : simple_type (s, e, f, container), base_type (s)
        {
        }

      public:
        time&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        time&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        time&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        time ()
            : base_type ()
        {
        }
      };


      // entity: ncname
      //
      template <typename C>
      class entity: public ncname<C>
      {
      protected:
        typedef ncname<C> base_type;
        typedef typename base_type::primary_type primary_type;

        base_type&
        base ()
        {
          return *this;
        }

      public:
        entity (const C* s)
            : base_type (s)
        {
        }

        entity (const C* s, std::size_t n)
            : base_type (s, n)
        {
        }

        entity (std::size_t n, C c)
            : base_type (n, c)
        {
        }

        entity (const primary_type& str)
            : base_type (str)
        {
        }

        entity (const primary_type& str,
                std::size_t pos,
                std::size_t n = primary_type::npos)
            : base_type (str, pos, n)
        {
        }

        template <typename S>
        entity (istream<S>&, flags = 0, type* container = 0);

      public:
        entity (const entity& other, flags f = 0, type* container = 0)
            : base_type (other, f, container)
        {
        }

        virtual entity*
        _clone (flags f = 0, type* container = 0) const
        {
          return new entity (*this, f, container);
        }

      public:
        entity (const xml::dom::element<C>& e, flags f, type* container)
            : base_type (e, f, container)
        {
        }

        entity (const xml::dom::attribute<C>& a, flags f, type* container)
            : base_type (a, f, container)
        {
        }

        entity (const std::basic_string<C>& s,
                const xml::dom::element<C>* e,
                flags f,
                type* container)
            : base_type (s, e, f, container)
        {
        }

      public:
        entity&
        operator= (C c)
        {
          base () = c;
          return *this;
        }

        entity&
        operator= (const C* s)
        {
          base () = s;
          return *this;
        }

        entity&
        operator= (const primary_type& str)
        {
          base () = str;
          return *this;
        }

        // Implicit copy assignment operator.
        //

      protected:
        entity ()
            : base_type ()
        {
        }
      };


      // entities: sequence<entity>
      //
      template <typename C>
      class entities: public simple_type, public sequence<entity<C> >
      {
        typedef sequence<entity<C> > base_type;

      public:
        entities ()
        {
        }

        template <typename S>
        entities (istream<S>&, flags = 0, type* container = 0);

        entities (const entities& other, flags f = 0, type* container = 0)
            : simple_type (other, f, container),
              base_type (other, f, container)
        {
        }

        virtual entities*
        _clone (flags f = 0, type* container = 0) const
        {
          return new entities (*this, f, container);
        }

      public:
        entities (const xml::dom::element<C>& e, flags f, type* container)
            : simple_type (e, f, container), base_type (e, f, container)
        {
        }

        entities (const xml::dom::attribute<C>& a, flags f, type* container)
            : simple_type (a, f, container), base_type (a, f, container)
        {
        }

        entities (const std::basic_string<C>& s,
                  const xml::dom::element<C>* e,
                  flags f,
                  type* container)
            : simple_type (s, e, f, container), base_type (s, e, f, container)
        {
        }
      };
    }
  }
}

#include <xsd/cxx/tree/types.txx>

#endif  // XSD_CXX_TREE_TYPES_HXX

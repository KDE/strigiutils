// file      : xsd/cxx/parser/validating/parser.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cassert>

#include <xsd/cxx/parser/exceptions.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      namespace validating
      {

        // empty_content
        //


        template <typename C>
        void empty_content<C>::
        _start_any_element (const std::basic_string<C>&,
                            const std::basic_string<C>&)
        {
        }

        template <typename C>
        void empty_content<C>::
        _end_any_element (const std::basic_string<C>&,
                          const std::basic_string<C>&)
        {
        }

        template <typename C>
        void empty_content<C>::
        _any_attribute (const std::basic_string<C>&,
                        const std::basic_string<C>&,
                        const std::basic_string<C>&)
        {
        }

        template <typename C>
        void empty_content<C>::
        _any_characters (const std::basic_string<C>&)
        {
        }

        //
        //
        template <typename C>
        bool empty_content<C>::
        _start_element_impl (const std::basic_string<C>&,
                             const std::basic_string<C>&)
        {
          return false;
        }

        template <typename C>
        bool empty_content<C>::
        _end_element_impl (const std::basic_string<C>&,
                           const std::basic_string<C>&)
        {
          return false;
        }

        template <typename C>
        bool empty_content<C>::
        _attribute_impl (const std::basic_string<C>&,
                         const std::basic_string<C>&,
                         const std::basic_string<C>&)
        {
          return false;
        }

        template <typename C>
        bool empty_content<C>::
        _characters_impl (const std::basic_string<C>&)
        {
          return false;
        }

        //
        //
        template <typename C>
        void empty_content<C>::
        _start_element (const std::basic_string<C>& ns,
                        const std::basic_string<C>& name)
        {
          if (!_start_element_impl (ns, name))
            _unexpected_element (ns, name);
        }

        template <typename C>
        void empty_content<C>::
        _end_element (const std::basic_string<C>& ns,
                      const std::basic_string<C>& name)
        {
          if (!_end_element_impl (ns, name))
            _unexpected_element (ns, name);
        }

        template <typename C>
        void empty_content<C>::
        _attribute (const std::basic_string<C>& ns,
                    const std::basic_string<C>& name,
                    const std::basic_string<C>& value)
        {
          if (!_attribute_impl (ns, name, value))
            _any_attribute (ns, name, value); //@@ TODO
        }

        template <typename C>
        void empty_content<C>::
        _characters (const std::basic_string<C>& s)
        {
          if (!_characters_impl (s))
            _any_characters (s); //@@ TODO
        }

        //
        //
        template <typename C>
        void empty_content<C>::
        _expected_element (const std::basic_string<C>& en_ns,
                           const std::basic_string<C>& en_name)
        {
          throw expected_element<C> (en_ns, en_name);
        }

        template <typename C>
        void empty_content<C>::
        _expected_element (const std::basic_string<C>& en_ns,
                           const std::basic_string<C>& en_name,
                           const std::basic_string<C>& ex_ns,
                           const std::basic_string<C>& ex_name)
        {
          throw expected_element<C> (en_ns, en_name, ex_ns, ex_name);
        }

        template <typename C>
        void empty_content<C>::
        _unexpected_element (const std::basic_string<C>& ns,
                             const std::basic_string<C>& name)
        {
          throw unexpected_element<C> (ns, name);
        }

        template <typename C>
        void empty_content<C>::
        _unexpected_attribute (const std::basic_string<C>&,
                               const std::basic_string<C>&,
                               const std::basic_string<C>&)
        {
          // throw unexpected_attribute<C> (ns, name);
        }

        template <typename C>
        void empty_content<C>::
        _unexpected_characters (const std::basic_string<C>&)
        {
          // throw unexpected_characters<C> (ns, name);
        }

        //
        //
        template <typename C>
        void empty_content<C>::
        pre ()
        {
        }

        template <typename C>
        void empty_content<C>::
        _pre ()
        {
        }

        template <typename C>
        void empty_content<C>::
        _post ()
        {
        }


        // complex_content
        //


        template <typename C>
        void complex_content<C>::
        _start_element (const std::basic_string<C>& ns,
                        const std::basic_string<C>& name)
        {
          state& s (context_.back ());

          if (s.depth_++ > 0)
          {
            if (s.parser_)
              s.parser_->_start_element (ns, name);
          }
          else
          {
            if (!_start_element_impl (ns, name))
              _unexpected_element (ns, name);
            else if (s.parser_ != 0)
              s.parser_->_pre ();
          }
        }

        template <typename C>
        void complex_content<C>::
        _end_element (const std::basic_string<C>& ns,
                      const std::basic_string<C>& name)
        {
          // To understand what's going on here it is helpful to think of
          // a "total depth" as being the sum of individual depths over
          // all elements.
          //

          if (context_.back ().depth_ == 0)
          {
            state& s (*(++context_.rbegin ())); // One before last.

            if (--s.depth_ > 0)
            {
              // Indirect recursion.
              //
              if (s.parser_)
                s.parser_->_end_element (ns, name);
            }
            else
            {
              // Direct recursion.
              //
              assert (this == s.parser_);

              this->_post ();

              if (!_end_element_impl (ns, name))
                assert (false);
            }
          }
          else
          {
            state& s (context_.back ());

            if (--s.depth_ > 0)
            {
              if (s.parser_)
                s.parser_->_end_element (ns, name);
            }
            else
            {
              if (s.parser_ != 0)
                s.parser_->_post ();

              if (!_end_element_impl (ns, name))
                _unexpected_element (ns, name);
            }
          }
        }

        template <typename C>
        void complex_content<C>::
        _attribute (const std::basic_string<C>& ns,
                    const std::basic_string<C>& name,
                    const std::basic_string<C>& value)
        {
          state& s (context_.back ());

          if (s.depth_ > 0)
          {
            if (s.parser_)
              s.parser_->_attribute (ns, name, value);
          }
          else
          {
            if (!_attribute_impl (ns, name, value))
              _any_attribute (ns, name, value); //@@ TODO
          }
        }

        template <typename C>
        void complex_content<C>::
        _characters (const std::basic_string<C>& str)
        {
          state& s (context_.back ());

          if (s.depth_ > 0)
          {
            if (s.parser_)
              s.parser_->_characters (str);
          }
          else
          {
            if (!_characters_impl (str))
              _any_characters (str); // @@ TODO
          }
        }

        template <typename C>
        void complex_content<C>::
        _pre ()
        {
          context_.push_back (state ());
          _pre_impl ();
        }

        template <typename C>
        void complex_content<C>::
        _post ()
        {
          _post_impl ();
          context_.pop_back ();
        }

        template <typename C>
        void complex_content<C>::
        _pre_impl ()
        {
        }

        template <typename C>
        void complex_content<C>::
        _post_impl ()
        {
        }
      }
    }
  }
}

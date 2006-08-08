// file      : xsd/cxx/parser/non-validating/parser.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#include <cassert>

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
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
            _start_any_element (ns, name);
        }

        template <typename C>
        void empty_content<C>::
        _end_element (const std::basic_string<C>& ns,
                      const std::basic_string<C>& name)
        {
          if (!_end_element_impl (ns, name))
            _end_any_element (ns, name);
        }

        template <typename C>
        void empty_content<C>::
        _attribute (const std::basic_string<C>& ns,
                    const std::basic_string<C>& name,
                    const std::basic_string<C>& value)
        {
          if (!_attribute_impl (ns, name, value))
            _any_attribute (ns, name, value);
        }

        template <typename C>
        void empty_content<C>::
        _characters (const std::basic_string<C>& s)
        {
          if (!_characters_impl (s))
            _any_characters (s);
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
              _start_any_element (ns, name);
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
                _end_any_element (ns, name);
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
              _any_attribute (ns, name, value);
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
              _any_characters (str);
          }
        }

        template <typename C>
        void complex_content<C>::
        _pre ()
        {
          context_.push_back (state ());
        }

        template <typename C>
        void complex_content<C>::
        _post ()
        {
          context_.pop_back ();
        }
      }
    }
  }
}

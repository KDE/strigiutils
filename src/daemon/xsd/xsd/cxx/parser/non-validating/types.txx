// file      : xsd/cxx/parser/non-validating/types.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      namespace non_validating
      {

        // list
        //

        template <typename X, typename C>
        bool list<X, C>::
        _characters_impl (const std::basic_string<C>& s)
        {
          if (item_ == 0)
            return true;

          //@@ In case of huge lists, it would be more efficient
          //   not to use "total" consolidation. Instead consolidate
          //   here until the next space.
          //

          // Note that according to the spec the separator is exactly
          // one space (0x20). This makes our life much easier.
          //

          for (std::size_t i (0), j (s.find (C (' ')));;)
          {
            if (j != std::basic_string<C>::npos)
            {
              item_->pre ();
              item_->_pre ();
              item_->_characters (std::basic_string<C> (s, i, j - i));
              item_->_post ();
              item (item_->post ());

              i = j + 1;
              j = s.find (C (' '), i);
            }
            else
            {
              // Last element.
              //
              item_->pre ();
              item_->_pre ();
              item_->_characters (std::basic_string<C> (s, i));
              item_->_post ();
              item (item_->post ());

              break;
            }
          }

          return true;
        }

        template <typename C>
        bool list<void, C>::
        _characters_impl (const std::basic_string<C>& s)
        {
          if (item_ == 0)
            return true;

          for (std::size_t i (0), j (s.find (C (' ')));;)
          {
            if (j != std::basic_string<C>::npos)
            {
              item_->pre ();
              item_->_pre ();
              item_->_characters (std::basic_string<C> (s, i, j - i));
              item_->_post ();
              item_->post ();
              item ();

              i = j + 1;
              j = s.find (C (' '), i);
            }
            else
            {
              // Last element.
              //
              item_->pre ();
              item_->_pre ();
              item_->_characters (std::basic_string<C> (s, i));
              item_->_post ();
              item_->post ();
              item ();

              break;
            }
          }

          return true;
        }
      }
    }
  }
}

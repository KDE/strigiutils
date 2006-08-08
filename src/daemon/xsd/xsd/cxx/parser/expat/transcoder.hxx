// file      : xsd/cxx/parser/expat/transcoder.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2006 Code Synthesis Tools CC
// license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#ifndef XSD_CXX_PARSER_EXPAT_TRANSCODER_HXX
#define XSD_CXX_PARSER_EXPAT_TRANSCODER_HXX

#include <string>
#include <cstdlib> // std::size_t

// size  encoding
//
// 1     UTF-8
// 2     UTF-16 (or UCS-2)
// 4     UTF-32 (or UCS-4)
//

namespace xsd
{
  namespace cxx
  {
    namespace parser
    {
      namespace expat
      {
        template<typename S,
                 typename D,
                 std::size_t = sizeof (S),
                 std::size_t = sizeof (D)>
        class transcoder;


        //
        //
        template<typename S, typename D, std::size_t size>
        class transcoder <S, D, size, size>
        {
          typedef std::basic_string<D> string;

        public:
          static string
          transcode (const S* s)
          {
            size_t n (0);
            for (; s[n] != S (0); ++n);

            return transcode (s, n);
          };

          static string
          transcode (const S* s, std::size_t n)
          {
            return string (reinterpret_cast<const D*> (s), n);
          };

          // Assign.
          //
          static void
          assign (string& d, const S* s)
          {
            std::size_t n (0);
            for (; s[n] != S (0); ++n);

            assign (d, s, n);
          };

          static void
          assign (string& d, const S* s, std::size_t n)
          {
            d.assign (reinterpret_cast<const D*> (s), n);
          };


          // Append.
          //
          static void
          append (string& d, const S* s)
          {
            std::size_t n (0);
            for (; s[n] != S (0); ++n);

            append (d, s, n);
          };

          static void
          append (string& d, const S* s, std::size_t n)
          {
            d.append (reinterpret_cast<const D*> (s), n);
          };
        };
      }
    }
  }
}

#endif  // XSD_CXX_PARSER_EXPAT_TRANSCODER_HXX


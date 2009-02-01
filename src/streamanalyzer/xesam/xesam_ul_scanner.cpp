/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "xesam_ul_scanner.h"
#include "xesam_ul_parser.hh"
#include "strigilogging.h"

#include <string>

using std::string;

XesamUlScanner::XesamUlScanner()
{
  m_modifier = false;
  m_quotmarkClosed = false;
  m_quotmarkCount = 0;
}

int XesamUlScanner::yylex(YYSTYPE* yylval)
{
  string read;
  char ch;
  
  if (!is_open()) {
    STRIGI_LOG_ERROR ("XesamUlScanner::yylex", "file is not open")
    return -1;
  }
  else if (eof()) {
    STRIGI_LOG_DEBUG ("XesamUlScanner::yylex", "yy::xesam_ul_parser::token::END")
    return yy::xesam_ul_parser::token::END;
  }
  
  do {
    if (eof()) {
      m_modifier = false;
      return yy::xesam_ul_parser::token::END;
    }
    else
      ch = getCh();

    if ((m_modifier) && (isspace(ch) != 0))
      m_modifier = false;
    
  } while (isspace (ch) != 0);
  
  if (isalnum(ch) != 0) {

    // quotation mark closed and no space after it--> read modifiers
    if ((m_quotmarkClosed) && (m_modifier)) {
      if ( (ch == 'b') /*Boost*/ ||
            (ch == 'c') /*Case sensitive*/ ||
            (ch == 'C') /*Case insensitive*/ ||
            (ch == 'd') /*Diacritic sensitive*/ ||
            (ch == 'D') /*Diacritic insensitive*/ ||
            (ch == 'e') /*Exact match. Short for cdl*/ ||
            (ch == 'f') /*Fuzzy search*/ ||
            (ch == 'l') /*Don't do stemming*/ ||
            (ch == 'L') /*Do stemming*/ ||
            (ch == 'o') /*Ordered words*/ ||
            (ch == 'p') /*Proximity search (suggested default: 10)*/ ||
            (ch == 'r') /*The phrase is a regular expression*/ ||
            (ch == 's') /*Sloppy search*/ ||
            (ch == 'w') /*Word based matching*/ )
      {
        *yylval = ch;
        STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                          "yy::xesam_ul_parser::token::MODIFIER")
        return yy::xesam_ul_parser::token::MODIFIER;
      }
      else {
        //error, unknown modifier
        STRIGI_LOG_ERROR ("xesam_ul_scanner",
                          string("unknown modifier: |") + ch + '|')

        // don't interrupt parsing, just provide an empty modifier
        *yylval = "";
        return yy::xesam_ul_parser::token::MODIFIER;
      }
    }
    

    read = ch;
    while (!eof() && (isalnum (peekCh()) != 0)) {
      ch = getCh();
      read += ch;
    }

    //yy::xesam_ul_parser::token::KEYWORD (type)|(format)|(creator)|(tag)|(size)
    if ( (read.compare("type") == 0) ||
          (read.compare("format") == 0) ||
          (read.compare("creator") == 0) ||
          (read.compare("tag") == 0) ||
          (read.compare("size") == 0))
    {
      // set yylval
      *yylval = read;
      STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                        "yy::xesam_ul_parser::token::KEYWORD")
      return yy::xesam_ul_parser::token::KEYWORD;
    }
    else if ((read.compare("and") == 0) ||
              (read.compare("AND") == 0))
    {
      //yy::xesam_ul_parser::token::AND (and)|(AND)
      // set yylval
      *yylval = read;
      STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                        "yy::xesam_ul_parser::token::AND")
      return yy::xesam_ul_parser::token::AND;
    }
    else if ((read.compare("or") == 0) ||
              (read.compare("OR") == 0))
    {
      //yy::xesam_ul_parser::token::OR (or)|(OR)
      // set yylval
      *yylval = read;
      STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                        "yy::xesam_ul_parser::token::OR")
      return yy::xesam_ul_parser::token::OR;
    }
    else if (!eof()) {
      // read also symbols composing a WORD (except for ")
      char next = peekCh();
      while (!eof() && (isspace (next) == 0) && (next != '"')) {
        ch = getCh();
        read += ch;
        next = peekCh();
      }
      
      *yylval = read;
      STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                        "yy::xesam_ul_parser::token::WORD")
      return yy::xesam_ul_parser::token::WORD;
    }
  }
  else if ((ch == '=') || (ch == '<') || (ch == '>') || (ch == ':')) {
    // yy::xesam_ul_parser::token::RELATION (=)|(<=)|(>=)|(<)|(>)|(:)
    read = ch;

    // we've to look after <= and >=
    if (((ch == '<') || (ch == '>')) && !eof() && (peekCh() == '=')) {
      ch = getCh();
      read += ch;
    }

    // set yylval
    *yylval = read;
    STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                      "yy::xesam_ul_parser::token::RELATION")
    return yy::xesam_ul_parser::token::RELATION;
  }
  else if ((ch == '"')) {
    // yy::xesam_ul_parser::token::QUOTMARK (")
    read = ch;
    // set yylval
    *yylval = "";
    m_quotmarkCount++;
    if (m_quotmarkCount %2 == 0) {
      m_quotmarkClosed = true;
      m_modifier = true;
      m_quotmarkCount = 0;
      STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                        "yy::xesam_ul_parser::token::QUOTMARKCLOSE")
      return yy::xesam_ul_parser::token::QUOTMARKCLOSE;
    }
    else {
      m_quotmarkClosed = false;
      m_modifier = false;
      STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                        "yy::xesam_ul_parser::token::QUOTMARKOPEN")
      return yy::xesam_ul_parser::token::QUOTMARKOPEN;
    }
  }
  else if ((ch == '-')) {
    // yy::xesam_ul_parser::token::MINUS (-)
    read = ch;
    // set yylval
    *yylval = read;
    STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                      "yy::xesam_ul_parser::token::MINUS")
    return yy::xesam_ul_parser::token::MINUS;
  }
  else if ((ch == '+')) {
    // yy::xesam_ul_parser::token::MINUS (-)
    read = ch;
    // set yylval
    *yylval = read;
    STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                      "yy::xesam_ul_parser::token::PLUS")
    return yy::xesam_ul_parser::token::PLUS;
  }
  else if ((ch == '&') && !eof() && (peekCh() == '&')) {
    // yy::xesam_ul_parser::token::AND (&&)
    ch = getCh();
    read = ch + ch;
    // set yylval
    *yylval = read;
    STRIGI_LOG_DEBUG ("XesamUlScanner::yylex",
                      "yy::xesam_ul_parser::token::AND")
    return yy::xesam_ul_parser::token::AND;
  }
  else if ((ch == '|') && !eof() && (peekCh() == '|')) {
    // yy::xesam_ul_parser::token::OR (||)
    ch = getCh();
    read = ch + ch;
    // set yylval
    *yylval = read;
    STRIGI_LOG_DEBUG ("XesamUlScanner::yylex", "yy::xesam_ul_parser::token::OR")
    return yy::xesam_ul_parser::token::OR;
  }

  //unknown char!
  //TODO yyerror?
  STRIGI_LOG_DEBUG ("XesamUlScanner::yylex", "unknown char, returning -1")
  return -1;
}



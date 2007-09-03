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

%skeleton "lalr1.cc"
%defines
%define "parser_class_name" "xesam_ul_parser"

%{
#include <string>

#include "xesam_ul_driver.hh"
#include "xesam_ul_scanner.h"
#include "query.h"
class XesamUlDriver;
class XesamUlScanner;

namespace Strigi{
  class Query;
}

using std::cout;
using std::endl;
%}

%parse-param { XesamUlDriver* driver }
%lex-param { XesamUlDriver* driver }

%debug
%error-verbose

%token WORD

%token KEYWORD RELATION MODIFIER

// collectors
%token AND OR

%token MINUS
%token QUOTMARKOPEN QUOTMARKCLOSE

%token END 0 "end of file"

// definizione token iniziale
%start start

%%

// regole grammaticali

start: query END {cout << "query building finished" << endl;};

query:  symbol select r_query;

symbol: /* empty */ { driver->setNegate (false); }
        | MINUS
          {//TODO: hanlde
            cout << "symbol minus" << endl;
            driver->setNegate (true);
          };

select: text {
          cout << "select1\n";
          // just set term
          Strigi::Query* query = new Strigi::Query();
          query->term().setValue($1);

          // set symbol value
          query->setNegate(driver->negate());

          // set modifiers
          if (!driver->modifiers().empty()) {
            driver->applyModifiers (query, driver->modifiers());
            // reset modifiers
            driver->resetModifiers();
          }

          driver->addQuery (query);
        }
        | KEYWORD RELATION text {
          cout << "select2\n";
          Strigi::Query* query =new Strigi::Query();

          // set symbol value
          query->setNegate(driver->negate());
          
          // set KEYWORD
          query->fields().push_back($1);
          // set RELATION
          if ($2.compare(">") == 0)
            query->setType(Strigi::Query::GreaterThan);
          else if ($2.compare(">=") == 0)
            query->setType(Strigi::Query::GreaterThanEquals);
          else if ($2.compare("<") == 0)
            query->setType(Strigi::Query::LessThan);
          else if ($2.compare("<=") == 0)
            query->setType(Strigi::Query::LessThanEquals);
          else if (($2.compare("=") == 0) ||
                    ($2.compare(":") == 0))
            query->setType(Strigi::Query::Equals);
          // set text
          query->term().setValue($3);

          // set modifiers
          if (!driver->modifiers().empty()) {
            driver->applyModifiers (query, driver->modifiers());
            // reset modifiers
            driver->resetModifiers();
          }
          
          driver->addQuery(query);
        };

text: WORD | phrase;

r_query:  /*empty*/
          | collector {
              cout << "r_query1\n";
              Strigi::Query::Type collectorType;

              if ($1.compare("OR" == 0))
                collectorType = Strigi::Query::Or;
              else
                collectorType = Strigi::Query::And;
          
              Strigi::Query* query = driver->query();
              if ((query) && (query->type() != collectorType)){
                Strigi::Query* superQuery = new Strigi::Query();
                superQuery->setType(collectorType);
                superQuery->subQueries().push_back(*query);
                delete query;
                driver->setQuery (superQuery);
              }
              else if (!query) {
                Strigi::Query* superQuery = new Strigi::Query();
                superQuery->setType(collectorType);
                driver->setQuery (superQuery);
              }
              // if ((query) && (query->type() == collectorType)) do nothing,
              // query's semantic rule will put the new query as inside
              // driver->query().subQueries() and that's right

              // reset query modifiers
              driver->resetModifiers();
            }query
          | {
              cout << "r_query2\n";

              Strigi::Query* query = driver->query();
              if ((query) && (query->type() != Strigi::Query::And)){
                Strigi::Query* superQuery = new Strigi::Query();
                superQuery->setType(Strigi::Query::And);
                superQuery->subQueries().push_back(*query);
                delete query;
                driver->setQuery (superQuery);
              }
              else if (!query) {
                Strigi::Query* superQuery = new Strigi::Query();
                superQuery->setType(Strigi::Query::And);
                driver->setQuery (superQuery);
              }
              // if ((query) && (query->type() == collectorType)) do nothing,
              // query's semantic rule will put the new query as inside
              // driver->query().subQueries() and that's right

              // reset query modifiers
              driver->resetModifiers();
            } query;

phrase: QUOTMARKOPEN phrase_arg QUOTMARKCLOSE modifiers {
          $$ = $2;
          
          cout << "result phrase: " << $$ << endl;
          cout << "modifiers: " << $4 << endl;
          driver->setModifiers ($4);
        };

phrase_arg: /*empty */ {$$ = ""; }
            | phrase_arg WORD
              {
                $$ = "";
                if (!$1.empty())
                  $$ = $1 + " ";
                $$ += $2;
                cout << "result phrase_arg: |" << $$ << "|\n";
              };

modifiers:  /* empty */ { $$ = ""}
            | modifiers MODIFIER {
                $$ = "";
                if (!$1.empty())
                  $$ = $1;
                $$ += $2;
              };

collector:  AND { $$="AND"; }
            | OR {$$ = "OR"} ;

%%

int yy::yylex(YYSTYPE *yylval, XesamUlDriver* driver)//, yy::location *yylloc, XesamUlDriver* driver)
{
  XesamUlScanner* scanner = driver->scanner();
  yylval->clear();
  int ret = scanner->yylex(yylval);
  cout << "calling scanner yylval==|" << *yylval << "|, ret==|" << ret << "|\n";
  
  return ret;
}

void yy::xesam_ul_parser::error (const yy::location& yyloc,
                                 const std::string& error)
{
  std::cerr << "error: " << error << endl;
  driver->setError();
}

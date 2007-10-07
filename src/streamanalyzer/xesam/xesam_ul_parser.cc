/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


#include "xesam_ul_parser.hh"

/* User implementation prologue.  */
#include <stdio.h>


/* Line 317 of lalr1.cc.  */
#line 43 "xesam_ul_parser.cc"

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG							\
  for (bool yydebugcond_ = yydebug_; yydebugcond_; yydebugcond_ = false)	\
    (*yycdebug_)

/* Enable debugging if requested.  */
#if YYDEBUG

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab

namespace yy
{
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  xesam_ul_parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  xesam_ul_parser::xesam_ul_parser (XesamUlDriver* driver_yyarg)
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
      driver (driver_yyarg)
  {
  }

  xesam_ul_parser::~xesam_ul_parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  xesam_ul_parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  xesam_ul_parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif /* ! YYDEBUG */

  void
  xesam_ul_parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  xesam_ul_parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

  std::ostream&
  xesam_ul_parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  xesam_ul_parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  xesam_ul_parser::debug_level_type
  xesam_ul_parser::debug_level () const
  {
    return yydebug_;
  }

  void
  xesam_ul_parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }


  int
  xesam_ul_parser::parse ()
  {
    /// Look-ahead and look-ahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the look-ahead.
    semantic_type yylval;
    /// Location of the look-ahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;
    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without look-ahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a look-ahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, driver);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Accept?  */
    if (yyn == yyfinal_)
      goto yyacceptlab;

    /* Shift the look-ahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted unless it is eof.  */
    if (yychar != yyeof_)
      yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:
#line 69 "xesam_ul_parser.yy"
    {STRIGI_LOG_DEBUG ("xesam_ul_parser", "query building finished");}
    break;

  case 4:
#line 73 "xesam_ul_parser.yy"
    {
            STRIGI_LOG_ERROR ("xesam_ul_parser", "syntax error found, "
                              "forcing exit")
            Strigi::Query* query = driver->query();
            if (query) {
              delete query;
              driver->setQuery (0);
            }
          ;}
    break;

  case 5:
#line 83 "xesam_ul_parser.yy"
    { driver->setNegate (false); ;}
    break;

  case 6:
#line 85 "xesam_ul_parser.yy"
    {
            STRIGI_LOG_DEBUG ("xesam_ul_parser::symbol",
                              "minus --> negation enabled")
            driver->setNegate (true);
          ;}
    break;

  case 7:
#line 90 "xesam_ul_parser.yy"
    { /* when PLUS is found do nothing */ ;}
    break;

  case 8:
#line 92 "xesam_ul_parser.yy"
    {
          STRIGI_LOG_DEBUG ("xesam_ul_parser::select", "just text case")
          // just set term
          Strigi::Query* query = new Strigi::Query();
          query->term().setValue((yysemantic_stack_[(1) - (1)]));

          // set symbol value
          query->setNegate(driver->negate());

          // set modifiers
          if (!driver->modifiers().empty()) {
            driver->applyModifiers (query, driver->modifiers());
            // reset modifiers
            driver->resetModifiers();
          }

          driver->addQuery (query);
        ;}
    break;

  case 9:
#line 110 "xesam_ul_parser.yy"
    {
          STRIGI_LOG_DEBUG ("xesam_ul_parser::select",
                            "KEYWORD RELATION text case")
          Strigi::Query* query =new Strigi::Query();

          // set symbol value
          query->setNegate(driver->negate());
          
          // set KEYWORD
          query->fields().push_back((yysemantic_stack_[(3) - (1)]));
          // set RELATION
          if ((yysemantic_stack_[(3) - (2)]).compare(">") == 0)
            query->setType(Strigi::Query::GreaterThan);
          else if ((yysemantic_stack_[(3) - (2)]).compare(">=") == 0)
            query->setType(Strigi::Query::GreaterThanEquals);
          else if ((yysemantic_stack_[(3) - (2)]).compare("<") == 0)
            query->setType(Strigi::Query::LessThan);
          else if ((yysemantic_stack_[(3) - (2)]).compare("<=") == 0)
            query->setType(Strigi::Query::LessThanEquals);
          else if (((yysemantic_stack_[(3) - (2)]).compare("=") == 0) ||
                    ((yysemantic_stack_[(3) - (2)]).compare(":") == 0))
            query->setType(Strigi::Query::Equals);
          // set text
          query->term().setValue((yysemantic_stack_[(3) - (3)]));

          // set modifiers
          if (!driver->modifiers().empty()) {
            driver->applyModifiers (query, driver->modifiers());
            // reset modifiers
            driver->resetModifiers();
          }
          
          driver->addQuery(query);
        ;}
    break;

  case 13:
#line 148 "xesam_ul_parser.yy"
    {
              STRIGI_LOG_DEBUG ("xesam_ul_parser::r_query",
                            "collector specified")
              Strigi::Query::Type collectorType;

              if ((yysemantic_stack_[(1) - (1)]).compare("OR") == 0)
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
            ;}
    break;

  case 15:
#line 178 "xesam_ul_parser.yy"
    {
              STRIGI_LOG_DEBUG ("xesam_ul_parser::r_query",
                                "no collector specified")

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
            ;}
    break;

  case 17:
#line 203 "xesam_ul_parser.yy"
    {
          (yyval) = (yysemantic_stack_[(4) - (2)]);
          driver->setModifiers ((yysemantic_stack_[(4) - (4)]));
          STRIGI_LOG_DEBUG ("xesam_ul_parser::phrase",
                            std::string ("phrase value = |") + (yyval) + "|")
          STRIGI_LOG_DEBUG ("xesam_ul_parser::phrase",
                            std::string ("modifiers= |") + (yysemantic_stack_[(4) - (4)]) + "|")
        ;}
    break;

  case 18:
#line 212 "xesam_ul_parser.yy"
    {(yyval) = ""; ;}
    break;

  case 19:
#line 214 "xesam_ul_parser.yy"
    {
                (yyval) = "";
                if (!(yysemantic_stack_[(2) - (1)]).empty())
                  (yyval) = (yysemantic_stack_[(2) - (1)]) + " ";
                (yyval) += (yysemantic_stack_[(2) - (2)]);
              ;}
    break;

  case 20:
#line 221 "xesam_ul_parser.yy"
    { (yyval) = "";}
    break;

  case 21:
#line 222 "xesam_ul_parser.yy"
    {
                (yyval) = "";
                if (!(yysemantic_stack_[(2) - (1)]).empty())
                  (yyval) = (yysemantic_stack_[(2) - (1)]);
                (yyval) += (yysemantic_stack_[(2) - (2)]);
              ;}
    break;

  case 22:
#line 229 "xesam_ul_parser.yy"
    { (yyval)="AND"; ;}
    break;

  case 23:
#line 230 "xesam_ul_parser.yy"
    {(yyval) = "OR";}
    break;


    /* Line 675 of lalr1.cc.  */
#line 600 "xesam_ul_parser.cc"
	default: break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse look-ahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    if (yyn == yyfinal_)
      goto yyacceptlab;

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the look-ahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		   &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyeof_ && yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  xesam_ul_parser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char xesam_ul_parser::yypact_ninf_ = -6;
  const signed char
  xesam_ul_parser::yypact_[] =
  {
        -1,    -6,    -6,    -6,     5,    -6,     3,    -6,    -6,    12,
      -6,    11,    -6,    -6,     9,     1,    -6,    -6,    16,    -1,
      -6,    -6,    -6,    -6,    -6,    -6,    -1,    -5,    -6,    -6
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  xesam_ul_parser::yydefact_[] =
  {
         0,     4,     6,     7,     0,     2,     0,     1,    10,     0,
      18,    15,     8,    11,     0,     0,    22,    23,     0,     0,
      13,     9,    19,    20,     3,    16,     0,    17,    14,    21
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  xesam_ul_parser::yypgoto_[] =
  {
        -6,    -6,    -4,    -6,    -6,     7,    -6,    -6,    -6,    -6,
      -6,    -6,    -6
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  xesam_ul_parser::yydefgoto_[] =
  {
        -1,     4,     5,     6,    11,    12,    18,    26,    19,    13,
      15,    27,    20
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char xesam_ul_parser::yytable_ninf_ = -13;
  const signed char
  xesam_ul_parser::yytable_[] =
  {
         1,    29,    -5,    -5,    22,     7,     8,     9,     2,     3,
      -5,   -12,     8,    23,    10,    25,    24,    14,    16,    17,
      10,    21,    28
  };

  /* YYCHECK.  */
  const unsigned char
  xesam_ul_parser::yycheck_[] =
  {
         1,     6,     3,     4,     3,     0,     3,     4,     9,    10,
      11,     0,     3,    12,    11,    19,     0,     5,     7,     8,
      11,    14,    26
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  xesam_ul_parser::yystos_[] =
  {
         0,     1,     9,    10,    14,    15,    16,     0,     3,     4,
      11,    17,    18,    22,     5,    23,     7,     8,    19,    21,
      25,    18,     3,    12,     0,    15,    20,    24,    15,     6
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  xesam_ul_parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  xesam_ul_parser::yyr1_[] =
  {
         0,    13,    14,    15,    15,    16,    16,    16,    17,    17,
      18,    18,    19,    20,    19,    21,    19,    22,    23,    23,
      24,    24,    25,    25
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  xesam_ul_parser::yyr2_[] =
  {
         0,     2,     1,     4,     1,     0,     1,     1,     1,     3,
       1,     1,     0,     0,     3,     0,     2,     4,     0,     2,
       0,     2,     1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const xesam_ul_parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "WORD", "KEYWORD", "RELATION",
  "MODIFIER", "AND", "OR", "MINUS", "PLUS", "QUOTMARKOPEN",
  "QUOTMARKCLOSE", "$accept", "start", "query", "symbol", "select", "text",
  "r_query", "@1", "@2", "phrase", "phrase_arg", "modifiers", "collector", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const xesam_ul_parser::rhs_number_type
  xesam_ul_parser::yyrhs_[] =
  {
        14,     0,    -1,    15,    -1,    16,    17,    19,     0,    -1,
       1,    -1,    -1,     9,    -1,    10,    -1,    18,    -1,     4,
       5,    18,    -1,     3,    -1,    22,    -1,    -1,    -1,    25,
      20,    15,    -1,    -1,    21,    15,    -1,    11,    23,    12,
      24,    -1,    -1,    23,     3,    -1,    -1,    24,     6,    -1,
       7,    -1,     8,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  xesam_ul_parser::yyprhs_[] =
  {
         0,     0,     3,     5,    10,    12,    13,    15,    17,    19,
      23,    25,    27,    28,    29,    33,    34,    37,    42,    43,
      46,    47,    50,    52
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  xesam_ul_parser::yyrline_[] =
  {
         0,    69,    69,    71,    72,    83,    84,    90,    92,   110,
     145,   145,   147,   148,   148,   178,   178,   203,   212,   213,
     221,   222,   229,   230
  };

  // Print the state stack on the debug stream.
  void
  xesam_ul_parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  xesam_ul_parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "), ";
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  xesam_ul_parser::token_number_type
  xesam_ul_parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int xesam_ul_parser::yyeof_ = 0;
  const int xesam_ul_parser::yylast_ = 22;
  const int xesam_ul_parser::yynnts_ = 13;
  const int xesam_ul_parser::yyempty_ = -2;
  const int xesam_ul_parser::yyfinal_ = 7;
  const int xesam_ul_parser::yyterror_ = 1;
  const int xesam_ul_parser::yyerrcode_ = 256;
  const int xesam_ul_parser::yyntokens_ = 13;

  const unsigned int xesam_ul_parser::yyuser_token_number_max_ = 267;
  const xesam_ul_parser::token_number_type xesam_ul_parser::yyundef_token_ = 2;

} // namespace yy

#line 232 "xesam_ul_parser.yy"


int yy::yylex(YYSTYPE *yylval, XesamUlDriver* driver)//, yy::location *yylloc, XesamUlDriver* driver)
{
  XesamUlScanner* scanner = driver->scanner();
  yylval->clear();
  int ret = scanner->yylex(yylval);

  char buff [50];
  snprintf (buff, 50 * sizeof (char), "%i", ret);

  STRIGI_LOG_DEBUG ("xesam_ul_parser::yylex",
                    std::string("calling scanner yylval==|") + *yylval
                    + "|, ret==|" + buff + "|")
  
  return ret;
}

void yy::xesam_ul_parser::error (const yy::location& yyloc,
                                 const std::string& error)
{
  STRIGI_LOG_ERROR ("xesam_ul_parser::error",
                    error)
  driver->setError();
}


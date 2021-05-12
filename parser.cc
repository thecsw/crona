// A Bison parser, made by GNU Bison 3.7.6.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "grammar.hh"


// Unqualified %code blocks.
#line 36 "crona.yy"

    // C std code for utility functions
    #include <iostream>
    #include <cstdlib>
    #include <fstream>
 
    // Our code for interoperation between scanner/parser
    #include "scanner.hpp"
    #include "ast.hpp"
    #include "tokens.hpp"

    //Request tokens from our scanner member, not 
    // from a global function
    #undef yylex
    #define yylex scanner.yylex

#line 63 "parser.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 5 "crona.yy"
namespace crona {
#line 137 "parser.cc"

  /// Build a parser object.
  Parser::Parser (crona::Scanner &scanner_yyarg, crona::ProgramNode** root_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      root (root_yyarg)
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/

  // basic_symbol.
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (semantic_type) v)
    : Base (t)
    , value (YY_MOVE (v))
  {}

  template <typename Base>
  Parser::symbol_kind_type
  Parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }

  template <typename Base>
  bool
  Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
  }

  // by_kind.
  Parser::by_kind::by_kind ()
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  Parser::by_kind::by_kind (by_kind&& that)
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  Parser::by_kind::by_kind (const by_kind& that)
    : kind_ (that.kind_)
  {}

  Parser::by_kind::by_kind (token_kind_type t)
    : kind_ (yytranslate_ (t))
  {}

  void
  Parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  Parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  Parser::symbol_kind_type
  Parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }

  Parser::symbol_kind_type
  Parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_kind_type
  Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    return *this;
  }

  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YY_USE (yysym.kind ());
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;


      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // program: globals
#line 170 "crona.yy"
                {
                    (yylhs.value.transProgram) = new ProgramNode((yystack_[0].value.transDeclList));
                    *root = (yylhs.value.transProgram);
                }
#line 592 "parser.cc"
    break;

  case 3: // globals: globals decl
#line 176 "crona.yy"
                {
                    (yylhs.value.transDeclList) = (yystack_[1].value.transDeclList); 
                    DeclNode * declNode = (yystack_[0].value.transDecl);
                    (yylhs.value.transDeclList)->push_back(declNode);
                }
#line 602 "parser.cc"
    break;

  case 4: // globals: %empty
#line 181 "crona.yy"
                                { (yylhs.value.transDeclList) = new std::list<DeclNode * >(); }
#line 608 "parser.cc"
    break;

  case 5: // decl: varDecl SEMICOLON
#line 184 "crona.yy"
                {
                    (yylhs.value.transDecl) = (yystack_[1].value.transVarDecl);
                }
#line 616 "parser.cc"
    break;

  case 6: // decl: fnDecl
#line 187 "crona.yy"
                         {
                    (yylhs.value.transDecl) = (yystack_[0].value.transFnDecl);
                }
#line 624 "parser.cc"
    break;

  case 7: // varDecl: id COLON type
#line 192 "crona.yy"
                {
                    size_t line = (yystack_[2].value.transID)->line();
                    size_t col = (yystack_[2].value.transID)->col();
                    (yylhs.value.transVarDecl) = new VarDeclNode(line, col, (yystack_[2].value.transID), (yystack_[0].value.transType));
                }
#line 634 "parser.cc"
    break;

  case 8: // type: INT
#line 198 "crona.yy"
                          { 
                    (yylhs.value.transType) = new IntTypeNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col()); 
                }
#line 642 "parser.cc"
    break;

  case 9: // type: INT ARRAY LBRACE INTLITERAL RBRACE
#line 201 "crona.yy"
                                                     { 
                    (yylhs.value.transType) = new ArrayTypeNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col(), new IntTypeNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col()), new IntLitNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col(), (yystack_[1].value.transIntToken)->num())); 
                }
#line 650 "parser.cc"
    break;

  case 10: // type: BOOL
#line 204 "crona.yy"
                       { 
                    (yylhs.value.transType) = new BoolTypeNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col());
                }
#line 658 "parser.cc"
    break;

  case 11: // type: BOOL ARRAY LBRACE INTLITERAL RBRACE
#line 207 "crona.yy"
                                                      {
                    (yylhs.value.transType) = new ArrayTypeNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col(), new BoolTypeNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col()), new IntLitNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col(), (yystack_[1].value.transIntToken)->num())); 
                }
#line 666 "parser.cc"
    break;

  case 12: // type: BYTE
#line 210 "crona.yy"
                       { 
                    (yylhs.value.transType) = new ByteTypeNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col()); 
                }
#line 674 "parser.cc"
    break;

  case 13: // type: BYTE ARRAY LBRACE INTLITERAL RBRACE
#line 213 "crona.yy"
                                                      {
                    (yylhs.value.transType) = new ArrayTypeNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col(), new ByteTypeNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col()), new IntLitNode((yystack_[4].value.transToken)->line(), (yystack_[4].value.transToken)->col(), (yystack_[1].value.transIntToken)->num())); 
                }
#line 682 "parser.cc"
    break;

  case 14: // type: STRING
#line 216 "crona.yy"
                         {
                    (yylhs.value.transType) = new ArrayTypeNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col(), new ByteTypeNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col()), new IntLitNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col(), 0));;
                }
#line 690 "parser.cc"
    break;

  case 15: // type: VOID
#line 219 "crona.yy"
                       {
                    (yylhs.value.transType) = new VoidTypeNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col());
                }
#line 698 "parser.cc"
    break;

  case 16: // fnDecl: id COLON type formals fnBody
#line 223 "crona.yy"
                                               {
                    (yylhs.value.transFnDecl) = new FnDeclNode((yystack_[4].value.transID)->line(), (yystack_[4].value.transID)->col(), (yystack_[4].value.transID), (yystack_[2].value.transType), (yystack_[1].value.transFormalsList), (yystack_[0].value.transStmtList));
                }
#line 706 "parser.cc"
    break;

  case 17: // formals: LPAREN RPAREN
#line 227 "crona.yy"
                                { 
                    (yylhs.value.transFormalsList) = new std::list<FormalDeclNode*>(); 
                }
#line 714 "parser.cc"
    break;

  case 18: // formals: LPAREN formalsList RPAREN
#line 230 "crona.yy"
                                            {
                    (yylhs.value.transFormalsList) = (yystack_[1].value.transFormalsList);
                }
#line 722 "parser.cc"
    break;

  case 19: // formalsList: formalDecl
#line 234 "crona.yy"
                             {
                    (yylhs.value.transFormalsList) = new std::list<FormalDeclNode*>();
                    (yylhs.value.transFormalsList)->push_front((yystack_[0].value.transFormalDecl));
                }
#line 731 "parser.cc"
    break;

  case 20: // formalsList: formalDecl COMMA formalsList
#line 238 "crona.yy"
                                               {
                    (yystack_[0].value.transFormalsList)->push_front((yystack_[2].value.transFormalDecl));
                    (yylhs.value.transFormalsList) = (yystack_[0].value.transFormalsList);
                }
#line 740 "parser.cc"
    break;

  case 21: // formalDecl: id COLON type
#line 243 "crona.yy"
                                {
                    (yylhs.value.transFormalDecl) = new FormalDeclNode((yystack_[2].value.transID)->line(), (yystack_[2].value.transID)->col(), (yystack_[2].value.transID), (yystack_[0].value.transType));
                }
#line 748 "parser.cc"
    break;

  case 22: // fnBody: LCURLY stmtList RCURLY
#line 247 "crona.yy"
                                         {
                    (yylhs.value.transStmtList) = (yystack_[1].value.transStmtList);
                }
#line 756 "parser.cc"
    break;

  case 23: // stmtList: stmtList stmt
#line 251 "crona.yy"
                                {
                    if (!(yystack_[1].value.transStmtList)) {
                        (yylhs.value.transStmtList) = new std::list<StmtNode*>();
                    } else {
                        (yylhs.value.transStmtList) = (yystack_[1].value.transStmtList);
                    }
                    (yylhs.value.transStmtList)->push_back((yystack_[0].value.transStmt));
                }
#line 769 "parser.cc"
    break;

  case 24: // stmtList: %empty
#line 259 "crona.yy"
                                {
                    (yylhs.value.transStmtList) = new std::list<StmtNode*>();
                }
#line 777 "parser.cc"
    break;

  case 25: // stmt: varDecl SEMICOLON
#line 263 "crona.yy"
                                    {
                    (yylhs.value.transStmt) = (yystack_[1].value.transVarDecl);    
                }
#line 785 "parser.cc"
    break;

  case 26: // stmt: assignExp SEMICOLON
#line 266 "crona.yy"
                                      {
                    (yylhs.value.transStmt) = new AssignStmtNode((yystack_[1].value.transAssignExp)->line(), (yystack_[1].value.transAssignExp)->col(), (yystack_[1].value.transAssignExp));
                }
#line 793 "parser.cc"
    break;

  case 27: // stmt: lval DASHDASH SEMICOLON
#line 269 "crona.yy"
                                          {
                    (yylhs.value.transStmt) = new PostDecStmtNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transLVal));
                }
#line 801 "parser.cc"
    break;

  case 28: // stmt: lval CROSSCROSS SEMICOLON
#line 272 "crona.yy"
                                            {
                    (yylhs.value.transStmt) = new PostIncStmtNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transLVal));
                }
#line 809 "parser.cc"
    break;

  case 29: // stmt: READ lval SEMICOLON
#line 275 "crona.yy"
                                      {
                    (yylhs.value.transStmt) = new ReadStmtNode((yystack_[2].value.transToken)->line(), (yystack_[2].value.transToken)->col(), (yystack_[1].value.transLVal));
                }
#line 817 "parser.cc"
    break;

  case 30: // stmt: WRITE exp SEMICOLON
#line 278 "crona.yy"
                                      {
                    (yylhs.value.transStmt) = new WriteStmtNode((yystack_[2].value.transToken)->line(), (yystack_[2].value.transToken)->col(), (yystack_[1].value.transExp));
                }
#line 825 "parser.cc"
    break;

  case 31: // stmt: IF LPAREN exp RPAREN LCURLY stmtList RCURLY
#line 281 "crona.yy"
                                                              {
                    (yylhs.value.transStmt) = new IfStmtNode((yystack_[6].value.transToken)->line(), (yystack_[6].value.transToken)->col(), (yystack_[4].value.transExp), (yystack_[1].value.transStmtList));
                }
#line 833 "parser.cc"
    break;

  case 32: // stmt: IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
#line 284 "crona.yy"
                                                                                          {
                    (yylhs.value.transStmt) = new IfElseStmtNode((yystack_[10].value.transToken)->line(), (yystack_[10].value.transToken)->col(), (yystack_[8].value.transExp), (yystack_[5].value.transStmtList), (yystack_[1].value.transStmtList));
                }
#line 841 "parser.cc"
    break;

  case 33: // stmt: WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
#line 287 "crona.yy"
                                                                 {
                    (yylhs.value.transStmt) = new WhileStmtNode((yystack_[6].value.transToken)->line(), (yystack_[6].value.transToken)->col(), (yystack_[4].value.transExp), (yystack_[1].value.transStmtList));
                }
#line 849 "parser.cc"
    break;

  case 34: // stmt: RETURN exp SEMICOLON
#line 290 "crona.yy"
                                       {
                    (yylhs.value.transStmt) = new ReturnStmtNode((yystack_[2].value.transToken)->line(), (yystack_[2].value.transToken)->col(), (yystack_[1].value.transExp));
                }
#line 857 "parser.cc"
    break;

  case 35: // stmt: RETURN SEMICOLON
#line 293 "crona.yy"
                                   {
                    (yylhs.value.transStmt) = new ReturnStmtNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), NULL);
                }
#line 865 "parser.cc"
    break;

  case 36: // stmt: fncall SEMICOLON
#line 296 "crona.yy"
                                   {
                    (yylhs.value.transStmt) = new CallStmtNode((yystack_[1].value.transFncall)->line(), (yystack_[1].value.transFncall)->col(), (yystack_[1].value.transFncall));
                }
#line 873 "parser.cc"
    break;

  case 37: // stmt: fnDecl
#line 299 "crona.yy"
                         { 
                    (yylhs.value.transStmt) = (yystack_[0].value.transFnDecl); 
                }
#line 881 "parser.cc"
    break;

  case 38: // assignExp: lval ASSIGN exp
#line 303 "crona.yy"
                                  {
                    (yylhs.value.transAssignExp) = new AssignExpNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transLVal), (yystack_[0].value.transExp));
                }
#line 889 "parser.cc"
    break;

  case 39: // fncall: id LPAREN RPAREN
#line 307 "crona.yy"
                                   {
                    (yylhs.value.transFncall) = new CallExpNode((yystack_[2].value.transID)->line(), (yystack_[2].value.transID)->col(), (yystack_[2].value.transID), new std::list<ExpNode*>());
                }
#line 897 "parser.cc"
    break;

  case 40: // fncall: id LPAREN actualList RPAREN
#line 310 "crona.yy"
                                              {
                    (yylhs.value.transFncall) = new CallExpNode((yystack_[3].value.transID)->line(), (yystack_[3].value.transID)->col(), (yystack_[3].value.transID), (yystack_[1].value.transActualList));
                }
#line 905 "parser.cc"
    break;

  case 41: // actualList: exp
#line 314 "crona.yy"
                      {
                    (yylhs.value.transActualList) = new std::list<ExpNode*>();
                    (yylhs.value.transActualList)->push_front((yystack_[0].value.transExp));
                }
#line 914 "parser.cc"
    break;

  case 42: // actualList: actualList COMMA exp
#line 318 "crona.yy"
                                       {
                    (yystack_[2].value.transActualList)->push_back((yystack_[0].value.transExp));
                    (yylhs.value.transActualList) = (yystack_[2].value.transActualList);
                }
#line 923 "parser.cc"
    break;

  case 43: // exp: assignExp
#line 323 "crona.yy"
                            {
                    (yylhs.value.transExp) = (yystack_[0].value.transAssignExp);
                }
#line 931 "parser.cc"
    break;

  case 44: // exp: exp DASH exp
#line 326 "crona.yy"
                               {
                    (yylhs.value.transExp) = new MinusNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 939 "parser.cc"
    break;

  case 45: // exp: exp CROSS exp
#line 329 "crona.yy"
                                {
                    (yylhs.value.transExp) = new PlusNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));   
                }
#line 947 "parser.cc"
    break;

  case 46: // exp: exp STAR exp
#line 332 "crona.yy"
                               {
                    (yylhs.value.transExp) = new TimesNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 955 "parser.cc"
    break;

  case 47: // exp: exp SLASH exp
#line 335 "crona.yy"
                                {
                    (yylhs.value.transExp) = new DivideNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 963 "parser.cc"
    break;

  case 48: // exp: exp AND exp
#line 338 "crona.yy"
                              {
                    (yylhs.value.transExp) = new AndNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 971 "parser.cc"
    break;

  case 49: // exp: exp OR exp
#line 341 "crona.yy"
                             {
                    (yylhs.value.transExp) = new OrNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 979 "parser.cc"
    break;

  case 50: // exp: exp EQUALS exp
#line 344 "crona.yy"
                                 {
                    (yylhs.value.transExp) = new EqualsNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 987 "parser.cc"
    break;

  case 51: // exp: exp NOTEQUALS exp
#line 347 "crona.yy"
                                    {
                    (yylhs.value.transExp) = new NotEqualsNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 995 "parser.cc"
    break;

  case 52: // exp: exp GREATER exp
#line 350 "crona.yy"
                                  {
                    (yylhs.value.transExp) = new GreaterNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 1003 "parser.cc"
    break;

  case 53: // exp: exp GREATEREQ exp
#line 353 "crona.yy"
                                    {
                    (yylhs.value.transExp) = new GreaterEqNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 1011 "parser.cc"
    break;

  case 54: // exp: exp LESS exp
#line 356 "crona.yy"
                               {
                    (yylhs.value.transExp) = new LessNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 1019 "parser.cc"
    break;

  case 55: // exp: exp LESSEQ exp
#line 359 "crona.yy"
                                 {
                    (yylhs.value.transExp) = new LessEqNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[2].value.transExp), (yystack_[0].value.transExp));
                }
#line 1027 "parser.cc"
    break;

  case 56: // exp: NOT exp
#line 362 "crona.yy"
                          {
                    (yylhs.value.transExp) = new NotNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[0].value.transExp));
                }
#line 1035 "parser.cc"
    break;

  case 57: // exp: DASH term
#line 365 "crona.yy"
                                      {
                    (yylhs.value.transExp) = new NegNode((yystack_[1].value.transToken)->line(), (yystack_[1].value.transToken)->col(), (yystack_[0].value.transExp));
                }
#line 1043 "parser.cc"
    break;

  case 58: // exp: term
#line 368 "crona.yy"
                       {
                    (yylhs.value.transExp) = (yystack_[0].value.transExp);
                }
#line 1051 "parser.cc"
    break;

  case 59: // term: lval
#line 372 "crona.yy"
                       {
                    (yylhs.value.transExp) = (yystack_[0].value.transLVal);
                }
#line 1059 "parser.cc"
    break;

  case 60: // term: INTLITERAL
#line 375 "crona.yy"
                             {
                    (yylhs.value.transExp) = new IntLitNode((yystack_[0].value.transIntToken)->line(), (yystack_[0].value.transIntToken)->col(), (yystack_[0].value.transIntToken)->num());
                }
#line 1067 "parser.cc"
    break;

  case 61: // term: STRLITERAL
#line 378 "crona.yy"
                             {
                    (yylhs.value.transExp) = new StrLitNode((yystack_[0].value.transStrToken)->line(), (yystack_[0].value.transStrToken)->col(), (yystack_[0].value.transStrToken)->str());
                }
#line 1075 "parser.cc"
    break;

  case 62: // term: TRUE
#line 381 "crona.yy"
                       {
                    (yylhs.value.transExp) = new TrueNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col());
                }
#line 1083 "parser.cc"
    break;

  case 63: // term: FALSE
#line 384 "crona.yy"
                        {
                    (yylhs.value.transExp) = new FalseNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col());
                }
#line 1091 "parser.cc"
    break;

  case 64: // term: HAVOC
#line 387 "crona.yy"
                        {
                    (yylhs.value.transExp) = new HavocNode((yystack_[0].value.transToken)->line(), (yystack_[0].value.transToken)->col());
                }
#line 1099 "parser.cc"
    break;

  case 65: // term: LPAREN exp RPAREN
#line 390 "crona.yy"
                                    {
                    (yylhs.value.transExp) = (yystack_[1].value.transExp);
                }
#line 1107 "parser.cc"
    break;

  case 66: // term: fncall
#line 393 "crona.yy"
                         {
                    (yylhs.value.transExp) = (yystack_[0].value.transFncall);
                }
#line 1115 "parser.cc"
    break;

  case 67: // lval: id
#line 397 "crona.yy"
                     {
                    (yylhs.value.transLVal) = (yystack_[0].value.transID);
                }
#line 1123 "parser.cc"
    break;

  case 68: // lval: id LBRACE exp RBRACE
#line 400 "crona.yy"
                                       {
                    (yylhs.value.transLVal) = new IndexNode((yystack_[3].value.transID)->line(), (yystack_[3].value.transID)->col(), (yystack_[3].value.transID), (yystack_[1].value.transExp));
                }
#line 1131 "parser.cc"
    break;

  case 69: // id: ID
#line 404 "crona.yy"
                         {
                    (yylhs.value.transID) = new IDNode((yystack_[0].value.transIDToken)->line(), (yystack_[0].value.transIDToken)->col(), (yystack_[0].value.transIDToken)->value());
                }
#line 1139 "parser.cc"
    break;


#line 1143 "parser.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
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
              else
                goto append;

            append:
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

  std::string
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // Parser::context.
  Parser::context::context (const Parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  Parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        int yychecklim = yylast_ - yyn + 1;
        int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }



  int
  Parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -55;

  const signed char Parser::yytable_ninf_ = -1;

  const short
  Parser::yypact_[] =
  {
     -55,     4,    -9,   -55,   -55,   -55,    -1,   -55,    20,   -55,
       8,    48,    64,    71,   -55,   -55,    42,    53,    54,    57,
      -6,    68,    65,    73,    88,   -55,    75,    81,   104,   -55,
     -55,    80,    82,    83,   -55,    -9,     8,     5,   -55,   -55,
     -55,   -55,   -55,    -9,    89,   -55,    51,    90,   -10,    85,
     -55,   -55,    87,    91,    61,     9,    92,    95,   -10,    67,
     -55,   -55,   -55,   -10,   -10,   -55,   -55,   -55,   -55,   -55,
     111,   -55,   112,    19,   -10,   142,   -55,   -55,   -55,   -10,
      93,    96,   -10,   304,   -55,   173,   -55,   -55,   200,   -55,
     -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10,
     -55,   -10,   -10,   227,   -55,   281,   -55,   -55,   254,   -55,
       7,   281,    99,   -55,   320,    66,    66,   326,   326,   326,
     326,   326,   326,   287,   -55,   -55,   101,   -55,   -10,   -55,
     -55,   -55,   281,   127,   172,   122,   -55,   114,   -55,   226,
     -55
  };

  const signed char
  Parser::yydefact_[] =
  {
       4,     0,     2,     1,    69,     3,     0,     6,     0,     5,
       0,    10,    12,     8,    14,    15,     7,     0,     0,     0,
       0,     0,     0,     0,     0,    17,     0,    19,     0,    24,
      16,     0,     0,     0,    18,     0,     0,     0,    11,    13,
       9,    20,    21,     0,     0,    22,     0,     0,     0,     0,
      37,    23,     0,     0,     0,    67,     0,    67,     0,     0,
      63,    64,    60,     0,     0,    35,    61,    62,    43,    66,
       0,    58,    59,    67,     0,     0,    25,    26,    36,     0,
       0,     0,     0,     0,    29,     0,    57,    59,     0,    56,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      34,     0,     0,     0,    30,    38,    28,    27,     0,    39,
       0,    41,     0,    65,    48,    45,    44,    50,    52,    53,
      54,    55,    51,    49,    47,    46,     0,    68,     0,    40,
      24,    24,    42,     0,     0,    31,    33,     0,    24,     0,
      32
  };

  const short
  Parser::yypgoto_[] =
  {
     -55,   -55,   -55,   -55,   139,   117,   148,   -55,   120,   -55,
     -55,   -54,   -55,   -34,   -32,   -55,   -37,    97,   -36,    -2
  };

  const signed char
  Parser::yydefgoto_[] =
  {
       0,     1,     2,     5,    49,    16,    50,    21,    26,    27,
      30,    37,    51,    68,    69,   110,    70,    71,    72,    73
  };

  const short
  Parser::yytable_[] =
  {
       8,    54,    59,    52,     3,    53,    60,    56,    61,     4,
       4,    75,    62,     4,    11,    12,   128,    10,    28,    63,
      64,    85,    43,    87,     4,    44,    88,    89,    10,    13,
      25,    66,    67,    28,    82,    55,     9,   103,    83,    45,
      46,    57,   105,   129,    82,   108,   111,    14,    83,    47,
      48,    15,    17,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    59,   124,   125,    79,    60,    18,    61,
       4,    20,    80,    62,    81,    19,   133,   134,    22,    23,
      63,    64,    24,    60,   139,    61,     4,    31,    65,    62,
      35,   132,    66,    67,    29,    32,    63,    54,    54,    52,
      52,    53,    53,    54,   101,    52,   102,    53,    66,    67,
      33,    34,    36,    38,    90,    39,    40,    79,    58,    74,
      82,    91,    76,    92,    77,   130,    93,   131,    78,    84,
     106,    55,    55,   107,    94,    95,   137,    55,    96,    97,
     138,     6,    98,    99,    43,    90,     4,    44,   100,   101,
       7,   102,    91,    42,    92,    41,    86,    93,     0,     0,
       0,   135,    46,     0,     0,    94,    95,     0,     0,    96,
      97,    47,    48,    98,    99,     0,    90,     0,     0,   104,
     101,     0,   102,    91,     0,    92,     0,     0,    93,    43,
       0,     4,    44,     0,     0,     0,    94,    95,     0,     0,
      96,    97,     0,    90,    98,    99,   136,    46,     0,   112,
      91,   101,    92,   102,     0,    93,    47,    48,     0,     0,
       0,     0,     0,    94,    95,     0,     0,    96,    97,     0,
      90,    98,    99,     0,     0,     0,   113,    91,   101,    92,
     102,     0,    93,    43,     0,     4,    44,     0,     0,     0,
      94,    95,     0,     0,    96,    97,     0,    90,    98,    99,
     140,    46,     0,   126,    91,   101,    92,   102,     0,    93,
      47,    48,     0,     0,     0,     0,     0,    94,    95,     0,
       0,    96,    97,     0,    90,    98,    99,   127,     0,     0,
      90,    91,   101,    92,   102,     0,    93,    91,     0,    92,
       0,     0,    93,     0,    94,    95,     0,     0,    96,    97,
      94,    95,    98,    99,    96,    97,    59,     0,    98,   101,
      60,   102,    61,     4,     0,   101,    62,   102,     0,     0,
      91,     0,    92,    63,    64,    93,    91,     0,    92,     0,
     109,    -1,     0,    94,    95,    66,    67,    96,    97,    -1,
      -1,    98,     0,    -1,    -1,     0,     0,    -1,   101,     0,
     102,     0,     0,     0,   101,     0,   102
  };

  const short
  Parser::yycheck_[] =
  {
       2,    37,    12,    37,     0,    37,    16,    43,    18,    19,
      19,    48,    22,    19,     6,     7,     9,     8,    20,    29,
      30,    58,    17,    59,    19,    20,    63,    64,     8,    21,
      36,    41,    42,    35,    25,    37,    37,    74,    29,    34,
      35,    43,    79,    36,    25,    82,    83,    39,    29,    44,
      45,    43,     4,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,    12,   101,   102,     5,    16,     4,    18,
      19,    29,    11,    22,    13,     4,   130,   131,    25,    25,
      29,    30,    25,    16,   138,    18,    19,    22,    37,    22,
       9,   128,    41,    42,    26,    22,    29,   133,   134,   133,
     134,   133,   134,   139,    38,   139,    40,   139,    41,    42,
      22,    36,     8,    33,     3,    33,    33,     5,    29,    29,
      25,    10,    37,    12,    37,    26,    15,    26,    37,    37,
      37,   133,   134,    37,    23,    24,    14,   139,    27,    28,
      26,     2,    31,    32,    17,     3,    19,    20,    37,    38,
       2,    40,    10,    36,    12,    35,    59,    15,    -1,    -1,
      -1,    34,    35,    -1,    -1,    23,    24,    -1,    -1,    27,
      28,    44,    45,    31,    32,    -1,     3,    -1,    -1,    37,
      38,    -1,    40,    10,    -1,    12,    -1,    -1,    15,    17,
      -1,    19,    20,    -1,    -1,    -1,    23,    24,    -1,    -1,
      27,    28,    -1,     3,    31,    32,    34,    35,    -1,    36,
      10,    38,    12,    40,    -1,    15,    44,    45,    -1,    -1,
      -1,    -1,    -1,    23,    24,    -1,    -1,    27,    28,    -1,
       3,    31,    32,    -1,    -1,    -1,    36,    10,    38,    12,
      40,    -1,    15,    17,    -1,    19,    20,    -1,    -1,    -1,
      23,    24,    -1,    -1,    27,    28,    -1,     3,    31,    32,
      34,    35,    -1,    36,    10,    38,    12,    40,    -1,    15,
      44,    45,    -1,    -1,    -1,    -1,    -1,    23,    24,    -1,
      -1,    27,    28,    -1,     3,    31,    32,    33,    -1,    -1,
       3,    10,    38,    12,    40,    -1,    15,    10,    -1,    12,
      -1,    -1,    15,    -1,    23,    24,    -1,    -1,    27,    28,
      23,    24,    31,    32,    27,    28,    12,    -1,    31,    38,
      16,    40,    18,    19,    -1,    38,    22,    40,    -1,    -1,
      10,    -1,    12,    29,    30,    15,    10,    -1,    12,    -1,
      36,    15,    -1,    23,    24,    41,    42,    27,    28,    23,
      24,    31,    -1,    27,    28,    -1,    -1,    31,    38,    -1,
      40,    -1,    -1,    -1,    38,    -1,    40
  };

  const signed char
  Parser::yystos_[] =
  {
       0,    47,    48,     0,    19,    49,    50,    52,    65,    37,
       8,     6,     7,    21,    39,    43,    51,     4,     4,     4,
      29,    53,    25,    25,    25,    36,    54,    55,    65,    26,
      56,    22,    22,    22,    36,     9,     8,    57,    33,    33,
      33,    54,    51,    17,    20,    34,    35,    44,    45,    50,
      52,    58,    59,    60,    64,    65,    64,    65,    29,    12,
      16,    18,    22,    29,    30,    37,    41,    42,    59,    60,
      62,    63,    64,    65,    29,    62,    37,    37,    37,     5,
      11,    13,    25,    29,    37,    62,    63,    64,    62,    62,
       3,    10,    12,    15,    23,    24,    27,    28,    31,    32,
      37,    38,    40,    62,    37,    62,    37,    37,    62,    36,
      61,    62,    36,    36,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    36,    33,     9,    36,
      26,    26,    62,    57,    57,    34,    34,    14,    26,    57,
      34
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    46,    47,    48,    48,    49,    49,    50,    51,    51,
      51,    51,    51,    51,    51,    51,    52,    53,    53,    54,
      54,    55,    56,    57,    57,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    59,    60,
      60,    61,    61,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    63,
      63,    63,    63,    63,    63,    63,    63,    64,    64,    65
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     1,     2,     0,     2,     1,     3,     1,     5,
       1,     5,     1,     5,     1,     1,     5,     2,     3,     1,
       3,     3,     3,     2,     0,     2,     2,     3,     3,     3,
       3,     7,    11,     7,     3,     2,     2,     1,     3,     3,
       4,     1,     3,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     4,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end file\"", "error", "\"invalid token\"", "AND", "ARRAY", "ASSIGN",
  "BOOL", "BYTE", "COLON", "COMMA", "CROSS", "CROSSCROSS", "DASH",
  "DASHDASH", "ELSE", "EQUALS", "FALSE", "READ", "HAVOC", "ID", "IF",
  "INT", "INTLITERAL", "GREATER", "GREATEREQ", "LBRACE", "LCURLY", "LESS",
  "LESSEQ", "LPAREN", "NOT", "NOTEQUALS", "OR", "RBRACE", "RCURLY",
  "RETURN", "RPAREN", "SEMICOLON", "SLASH", "STRING", "STAR", "STRLITERAL",
  "TRUE", "VOID", "WHILE", "WRITE", "$accept", "program", "globals",
  "decl", "varDecl", "type", "fnDecl", "formals", "formalsList",
  "formalDecl", "fnBody", "stmtList", "stmt", "assignExp", "fncall",
  "actualList", "exp", "term", "lval", "id", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  Parser::yyrline_[] =
  {
       0,   169,   169,   175,   181,   183,   187,   191,   198,   201,
     204,   207,   210,   213,   216,   219,   223,   227,   230,   234,
     238,   243,   247,   251,   259,   263,   266,   269,   272,   275,
     278,   281,   284,   287,   290,   293,   296,   299,   303,   307,
     310,   314,   318,   323,   326,   329,   332,   335,   338,   341,
     344,   347,   350,   353,   356,   359,   362,   365,   368,   372,
     375,   378,   381,   384,   387,   390,   393,   397,   400,   404
  };

  void
  Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  Parser::symbol_kind_type
  Parser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45
    };
    // Last valid token kind.
    const int code_max = 300;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return YY_CAST (symbol_kind_type, translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 5 "crona.yy"
} // crona
#line 1789 "parser.cc"

#line 408 "crona.yy"


void crona::Parser::error(const std::string& err_message){
    /* For project grading, only report "syntax error"
        if a program has bad syntax. However, you will
        probably want better output for debugging. Thus,
        this error function prints a verbose message to 
        stdout, but only prints "syntax error" to stderr
    */
	std::cout << err_message << std::endl;
	std::cerr << "syntax error" << std::endl;
}

%skeleton "lalr1.cc"
%require "3.2"
%debug
%defines
%define api.namespace {crona}
%define api.parser.class {Parser}
%define parse.assert
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
    #include "ast.hpp"
    namespace crona {
		class Scanner;
	}

//The following definition is required when 
// we don't use the %locations directive (which we won't)
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { crona::Scanner &scanner }
%parse-param { crona::ProgramNode** root }

%code{
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
}

/*
The %union directive is a way to specify the 
set of possible types that might be used as
translation attributes that a symbol might take.
For this project, only terminals have types (we'll
have translation attributes for non-terminals in the next
project)
*/
%union {
    crona::Token*                           lexeme;
    crona::Token*                           transToken;
    crona::IntLitToken*                     transIntToken;
    crona::StrToken*                        transStrToken;
    crona::IDToken*                         transIDToken;
    crona::ProgramNode*                     transProgram;
    std::list<crona::DeclNode *> *          transDeclList;
    crona::DeclNode *                       transDecl;
    crona::VarDeclNode *                    transVarDecl;
    crona::TypeNode *                       transType;
    crona::IDNode *                         transID;
    crona::FnDeclNode*                      transFnDecl;
    std::list<crona::FormalDeclNode*>*      transFormalsList;
    crona::FormalDeclNode*                  transFormalDecl;
    std::list<crona::StmtNode*>*            transStmtList;
    crona::StmtNode*                        transStmt;
    crona::AssignExpNode*                   transAssignExp;
    crona::CallExpNode*                     transFncall;
    std::list<crona::ExpNode*>*             transActualList;
    crona::ExpNode*                         transExp;
    crona::LValNode*                        transLVal;                             
}

%token                    END	   0 "end file"
%token	<transToken>      AND
%token	<transToken>      ARRAY
%token	<transToken>      ASSIGN
%token	<transToken>      BOOL
%token	<transToken>      BYTE
%token	<transToken>      COLON
%token	<transToken>      COMMA
%token	<transToken>      CROSS
%token	<transToken>      CROSSCROSS
%token	<transToken>      DASH
%token	<transToken>      DASHDASH
%token	<transToken>      ELSE
%token	<transToken>      EQUALS
%token	<transToken>      FALSE
%token	<transToken>      READ
%token	<transToken>      HAVOC
%token	<transIDToken>    ID
%token	<transToken>      IF
%token	<transToken>      INT
%token	<transIntToken>   INTLITERAL
%token	<transToken>      GREATER
%token	<transToken>      GREATEREQ
%token	<transToken>      LBRACE
%token	<transToken>      LCURLY
%token	<transToken>      LESS
%token	<transToken>      LESSEQ
%token	<transToken>      LPAREN
%token	<transToken>      NOT
%token	<transToken>      NOTEQUALS
%token	<transToken>      OR
%token	<transToken>      RBRACE
%token	<transToken>      RCURLY
%token	<transToken>      RETURN
%token	<transToken>      RPAREN
%token	<transToken>      SEMICOLON
%token	<transToken>      SLASH
%token	<transToken>      STRING
%token	<transToken>      STAR
%token	<transStrToken>   STRLITERAL
%token	<transToken>      TRUE
%token	<transToken>      VOID
%token	<transToken>      WHILE
%token	<transToken>      WRITE

/*  Nonterminals
 *  TODO: You will need to add more nonterminals
 *  to this list as you add productions to the grammar
 *  below (along with indicating the appropriate translation
 *  attribute type). Note that the specifier in brackets
 *  indicates the type of the translation attribute using
 *  the names defined in the %union directive above
 */
/*    (attribute type)      (nonterminal)    */
%type <transProgram>        program
%type <transDeclList>       globals
%type <transDecl>           decl
%type <transVarDecl>        varDecl
%type <transType>           type
%type <transID>             id
%type <transFnDecl>         fnDecl
%type <transFormalsList>    formals
%type <transFormalsList>    formalsList
%type <transFormalDecl>     formalDecl
%type <transStmtList>       fnBody
%type <transStmtList>       stmtList
%type <transStmt>           stmt
%type <transAssignExp>      assignExp
%type <transFncall>         fncall
%type <transActualList>     actualList
%type <transExp>            exp
%type <transExp>            term
%type <transLVal>           lval

%right ASSIGN
%left OR
%left AND
%nonassoc LESS LESSEQ EQUALS NOTEQUALS GREATER GREATEREQ
%left CROSS DASH
%left STAR SLASH
%left NOT

%%

program 	    : globals 
                {
                    $$ = new ProgramNode($1);
                    *root = $$;
                }

globals 	    : globals decl 
                {
                    $$ = $1; 
                    DeclNode * declNode = $2;
                    $$->push_back(declNode);
                }
                | /* epsilon */ { $$ = new std::list<DeclNode * >(); }

decl 		    : varDecl SEMICOLON 
                {
                    $$ = $1;
                }
                | fnDecl {
                    $$ = $1;
                }

varDecl 	    : id COLON type 
                {
                    size_t line = $1->line();
                    size_t col = $1->col();
                    $$ = new VarDeclNode(line, col, $1, $3);
                }

type 		    : INT { 
                    $$ = new IntTypeNode($1->line(), $1->col()); 
                }
                | INT ARRAY LBRACE INTLITERAL RBRACE { 
                    $$ = new ArrayTypeNode($1->line(), $1->col(), new IntTypeNode($1->line(), $1->col()), new IntLitNode($1->line(), $1->col(), $4->num())); 
                }
                | BOOL { 
                    $$ = new BoolTypeNode($1->line(), $1->col());
                }
                | BOOL ARRAY LBRACE INTLITERAL RBRACE {
                    $$ = new ArrayTypeNode($1->line(), $1->col(), new BoolTypeNode($1->line(), $1->col()), new IntLitNode($1->line(), $1->col(), $4->num())); 
                }
                | BYTE { 
                    $$ = new ByteTypeNode($1->line(), $1->col()); 
                }
                | BYTE ARRAY LBRACE INTLITERAL RBRACE {
                    $$ = new ArrayTypeNode($1->line(), $1->col(), new ByteTypeNode($1->line(), $1->col()), new IntLitNode($1->line(), $1->col(), $4->num())); 
                }
                | STRING {
                    $$ = new ArrayTypeNode($1->line(), $1->col(), new ByteTypeNode($1->line(), $1->col()), new IntLitNode($1->line(), $1->col(), 0));;
                }
                | VOID {
                    $$ = new VoidTypeNode($1->line(), $1->col());
                }

fnDecl          : id COLON type formals fnBody {
                    $$ = new FnDeclNode($1->line(), $1->col(), $1, $3, $4, $5);
                }

formals         : LPAREN RPAREN { 
                    $$ = new std::list<FormalDeclNode*>(); 
                }
                | LPAREN formalsList RPAREN {
                    $$ = $2;
                }

formalsList     : formalDecl {
                    $$ = new std::list<FormalDeclNode*>();
                    $$->push_front($1);
                }
                | formalDecl COMMA formalsList {
                    $3->push_front($1);
                    $$ = $3;
                }

formalDecl      : id COLON type {
                    $$ = new FormalDeclNode($1->line(), $1->col(), $1, $3);
                }

fnBody          : LCURLY stmtList RCURLY {
                    $$ = $2;
                }

stmtList        : stmtList stmt {
                    if (!$1) {
                        $$ = new std::list<StmtNode*>();
                    } else {
                        $$ = $1;
                    }
                    $$->push_back($2);
                }
                | /* epsilon */ {
                    $$ = new std::list<StmtNode*>();
                }

stmt            : varDecl SEMICOLON {
                    $$ = $1;    
                }
                | assignExp SEMICOLON {
                    $$ = new AssignStmtNode($1->line(), $1->col(), $1);
                }
                | lval DASHDASH SEMICOLON {
                    $$ = new PostDecStmtNode($2->line(), $2->col(), $1);
                }
                | lval CROSSCROSS SEMICOLON {
                    $$ = new PostIncStmtNode($2->line(), $2->col(), $1);
                }
                | READ lval SEMICOLON {
                    $$ = new ReadStmtNode($1->line(), $1->col(), $2);
                }
                | WRITE exp SEMICOLON {
                    $$ = new WriteStmtNode($1->line(), $1->col(), $2);
                }
                | IF LPAREN exp RPAREN LCURLY stmtList RCURLY {
                    $$ = new IfStmtNode($1->line(), $1->col(), $3, $6);
                }
                | IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY {
                    $$ = new IfElseStmtNode($1->line(), $1->col(), $3, $6, $10);
                }
                | WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY {
                    $$ = new WhileStmtNode($1->line(), $1->col(), $3, $6);
                }
                | RETURN exp SEMICOLON {
                    $$ = new ReturnStmtNode($1->line(), $1->col(), $2);
                }
                | RETURN SEMICOLON {
                    $$ = new ReturnStmtNode($1->line(), $1->col(), NULL);
                }
                | fncall SEMICOLON {
                    $$ = new CallStmtNode($1->line(), $1->col(), $1);
                }
                | fnDecl { 
                    $$ = $1; 
                }

assignExp       : lval ASSIGN exp {
                    $$ = new AssignExpNode($2->line(), $2->col(), $1, $3);
                }

fncall          : id LPAREN RPAREN {
                    $$ = new CallExpNode($1->line(), $1->col(), $1, new std::list<ExpNode*>());
                } 
                | id LPAREN actualList RPAREN {
                    $$ = new CallExpNode($1->line(), $1->col(), $1, $3);
                }

actualList      : exp {
                    $$ = new std::list<ExpNode*>();
                    $$->push_front($1);
                }
                | actualList COMMA exp {
                    $1->push_back($3);
                    $$ = $1;
                }
                
exp             : assignExp {
                    $$ = $1;
                }
                | exp DASH exp {
                    $$ = new MinusNode($2->line(), $2->col(), $1, $3);
                }
                | exp CROSS exp {
                    $$ = new PlusNode($2->line(), $2->col(), $1, $3);   
                }
                | exp STAR exp {
                    $$ = new TimesNode($2->line(), $2->col(), $1, $3);
                }
                | exp SLASH exp {
                    $$ = new DivideNode($2->line(), $2->col(), $1, $3);
                }
                | exp AND exp {
                    $$ = new AndNode($2->line(), $2->col(), $1, $3);
                }
                | exp OR exp {
                    $$ = new OrNode($2->line(), $2->col(), $1, $3);
                }
                | exp EQUALS exp {
                    $$ = new EqualsNode($2->line(), $2->col(), $1, $3);
                }
                | exp NOTEQUALS exp {
                    $$ = new NotEqualsNode($2->line(), $2->col(), $1, $3);
                }
                | exp GREATER exp {
                    $$ = new GreaterNode($2->line(), $2->col(), $1, $3);
                }
                | exp GREATEREQ exp {
                    $$ = new GreaterEqNode($2->line(), $2->col(), $1, $3);
                }
                | exp LESS exp {
                    $$ = new LessNode($2->line(), $2->col(), $1, $3);
                }
                | exp LESSEQ exp {
                    $$ = new LessEqNode($2->line(), $2->col(), $1, $3);
                }
                | NOT exp {
                    $$ = new NotNode($1->line(), $1->col(), $2);
                }
                | DASH term %prec NOT {
                    $$ = new NegNode($1->line(), $1->col(), $2);
                } 
                | term {
                    $$ = $1;
                }

term            : lval {
                    $$ = $1;
                }
                | INTLITERAL {
                    $$ = new IntLitNode($1->line(), $1->col(), $1->num());
                }
                | STRLITERAL {
                    $$ = new StrLitNode($1->line(), $1->col(), $1->str());
                }
                | TRUE {
                    $$ = new TrueNode($1->line(), $1->col());
                }
                | FALSE {
                    $$ = new FalseNode($1->line(), $1->col());
                }
                | HAVOC {
                    $$ = new HavocNode($1->line(), $1->col());
                }
                | LPAREN exp RPAREN {
                    $$ = $2;
                }
                | fncall {
                    $$ = $1;
                }

lval            : id {
                    $$ = $1;
                }
                | id LBRACE exp RBRACE {
                    $$ = new IndexNode($1->line(), $1->col(), $1, $3);
                }

id	            : ID {
                    $$ = new IDNode($1->line(), $1->col(), $1->value());
                }
	
%%

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

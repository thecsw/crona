# C-rona Language

Welcome! C-rona is a fantastic a general-purpose, procedural, 
imperative, and statically-typed programming language written
in C++

This repo contains a fully working compiler for x86_64 machines,
which itself was a semester-long project for EECS 665: Compiler
Construction, where we learned the art of building compilers from
scratch and this is the result.

I worked on the project with 
[Zachary Atkins](https://github.com/zatkins-dev) and without him
I wouldn't have gotten anywhere. We were great partners throughout
the whole semester. 

EECS 665: Compiler Construction was taught by fantastic
[Drew Davidson](https://ittc.ku.edu/~drew/),
who made probably one of the classicaly toughest courses in
undergraduate career into one of the most enjoyable classes
I have ever taken in my Computer Science undergrad.

What you see below is a language specification that was posted 
on our [special website](https://compilers.cool/language/) 
(no clue if it's still working, probably not, as each semester
gets their own language). Please find a faithful copy of the
language spec that follows.

## Lexical Details {#lexical-details .post-title}

::: {.post-description}
This section defines lexical details of the C-rona language.
:::

#### Reserved Words

The following words are considered *reserved*, and should be represented
yield a distinct token (rather than be counted as an identifier).

    int bool byte string void
    if else while return array
    true false read write havoc

#### Identifiers

Any sequence of one or more letters and/or digits, and/or underscores,
starting with a letter or underscore, should be treated as an identifier
token. Identifiers must not be reserved words, but may include a proper
substring that is a reserved word, e.g. `while1` is an identifier but
`while` is not.

#### Integer Literals

Any sequence of of one or more digits yields an integer literal token as
long as it is not part of an identifer or string.

#### String Literals

Any string literal (a sequence of zero or more *string* characters
surrounded by double quotes) should yield a string literal token. A
*string* character is either

-   an escaped character: a backslash followed by any one of the
    following characters:
    1.  n
    2.  t
    3.  a double quote
    4.  another backslash

    or
-   a single character other than newline or double quote or backslash.

Examples of legal string literals:

    ""
    "&!88"
    "use \n to denote a newline character"
    "use \" to  for a quote and \\ for a backslash"

Examples of things that are ***not*** legal string literals:

    "unterminated
    "also unterminated \"
    "backslash followed by space: \ is not allowed"
    "bad escaped character: \a AND not terminated

#### Symbol Operators

Any of the following one- or two-character symbols constitute a distinct
token:

    [   ]   {   }   (   )   ; :
    ,      --   -   +   ++     *   /
    !      ==   !=   <  >  
    <=   >=     =       &&    ||

#### Comments

Text starting with two slash characters (//) up to the end of the line
is a comment (except of course if those characters are inside a string
literal). For example:

    // this is a comment
    // and so is // this

The scanner should recognize and ignore comments (there is no COMMENT
token).

#### Whitespace

Spaces, tabs, and newline characters are whitespace. Whitespace
separates tokens and changes the character counter, but should otherwise
be ignored (except inside a string literal).

#### Illegal Characters

Any character that is not whitespace and is not part of a token or
comment is illegal.

#### Length Limits

No limit may be assumed on the lengths of identifiers, string literals,
integer literals, comments, etc. other than those limits imposed by the
underlying implementation of the compiler\'s host language.
:::

::: {#syntax .section .post}
## Syntactic Details {#syntactic-details .post-title}

::: {.post-description}
*This section described the syntax of the C-rona language.*
:::

#### Basics

The basic syntax of C-rona is designed to evoke a simplified varient C.
C-rona is a block-structured language, with most blocks delimited by
curly braces. Variables and functions may be declared in the global
scope, most statements and declarations are delimited semicolons.

#### Notable Differences from C

While the canonical reference for C-rona syntax is its context-free
grammar, there are a couple of \"standout\" points which deserve special
attention for their deviation from C:

-   Type specifiers are listed *after identifiers*. For example, the
    declaration `int i;` would look like `i : int;` in C-rona
-   Declarations are not allowed to have initializers. Thus,
    `int answer = 42;` is illegal in C-rona. Instead, initialization
    must be a separate statement, i.e. `answer : int; answer = 1;`.

#### Context-Free Grammar for C-rona

    /*********************************************************************
     Grammar for C-Rona programs
     ********************************************************************/
    program         ::= globals

    globals         ::= globals decl
                    | /* epsilon */

    decl            ::= varDecl SEMICOLON
                    | fnDecl

    varDecl         ::= id COLON type

    type            ::= INT
            |   INT ARRAY LBRACE INTLITERAL RBRACE
                    |   BOOL
            |   BOOL ARRAY LBRACE INTLITERAL RBRACE
                    |   BYTE
            |   BYTE ARRAY LBRACE INTLITERAL RBRACE
                    |   STRING
                    |   VOID

    fnDecl          ::= id COLON type formals fnBody

    formals         ::= LPAREN RPAREN
                    | LPAREN formalsList RPAREN

    formalsList     ::= formalDecl
                    | formalDecl COMMA formalsList

    formalDecl      ::= id COLON type

    fnBody          ::= LCURLY stmtList RCURLY

    stmtList        ::= stmtList stmt
                    | /* epsilon */

    stmt            ::= varDecl SEMICOLON
                    | assignExp SEMICOLON
                    | lval DASHDASH SEMICOLON
                    | lval CROSSCROSS SEMICOLON
                    | READ lval SEMICOLON
                    | WRITE exp SEMICOLON
                    | IF LPAREN exp RPAREN LCURLY stmtList RCURLY
                    | IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
                    | WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
                    | RETURN exp SEMICOLON
                    | RETURN SEMICOLON
                    | fncall SEMICOLON

    assignExp       ::= lval ASSIGN exp

    fncall          ::=  id LPAREN RPAREN   // fn call with no args
                    | id LPAREN actualsList RPAREN  // with args

    actualsList     ::= exp
                    | actualsList COMMA exp
                    
    exp             ::= assignExp
                    | exp DASH exp
                    | exp CROSS exp
                    | exp STAR exp
                    | exp SLASH exp
                    | exp AND exp
                    | exp OR exp
                    | exp EQUALS exp
                    | exp NOTEQUALS exp
                    | exp GREATER exp
                    | exp GREATEREQ exp
                    | exp LESS exp
                    | exp LESSEQ exp
                    | NOT exp
                    | DASH term
                    | term

    term            ::= lval
                    | INTLITERAL
                    | STRLITERAL
                    | TRUE
                    | FALSE
            | HAVOC
                    | LPAREN exp RPAREN
                    | fncall

    lval             ::= id
                    | id LBRACE exp RBRACE

    id              ::= ID
:::

::: {#types .section .post}
## Type System Details {#type-system-details .post-title}

::: {.post-description}
This section defines details of the C-rona type system.
:::

[Type Promotions]{#cascade .subsubhead}

Any type is promotable to itself. Additionally, `byte` is promotable to
`int`

[Operands]{#cascade .subsubhead}

The following shows the atomic operands and the types that they are
assigned:

-   numeric literals (e.g. 7, 3) are of type `byte` if their value is
    less than 256, `int` otherwise
-   bool literals (i.e. true, false) are of type `bool`
-   string literals are of array types of byte for their length + 1
    (e.g. \"hello\" is of type `array byte[6]`)
-   identifiers have the type of their declaration, which is determined
    during name analysis

[Operators]{#cascade .subsubhead}

The operators in the language are divided into the following categories:

-   **logical:** not, and, or
-   **arithmetic**: plus, minus, times, divide, negation, postincrement,
    postdecrement
-   **equality**: equals, not equals
-   **relational**: less than (\<), greater than (\>), less then or
    equals (\<=), greater than or equals (\>=)
-   **indexing**: index (`[ ]`)
-   **assignment**: assign (`=`)

The type rules of the language are as follows:

-   **logical operators and conditions** are legal if and only if:

    -   All operands are bool

    The result type is bool in legal cases, ERROR otherwise.

-   **arithmetic operations** are legal if and only if:

    -   Operands are both int - the result type is int
    -   Operands are both byte - the result type is byte
    -   Operands are a mix of int and byte - the byte operand will be
        promoted to int and the result type is int

    In all illegal cases, the result type is ERROR.

-   **relational operations** are legal if and only if:

    -   Both operands are promotable to `int` (i.e both are byte, both
        are int, or they are a mix of byte and int)

    The result type is bool in legal cases, ERROR otherwise.

-   **equality operations** are legal if and only if:

    -   Both operands are of the same type or promotable to the same
        type (i.e. int and byte)
    -   Neither operand is a function type
    -   Neither operand is an array

    The result type is bool in legal cases, ERROR otherwise.

-   ::: {#assignType}
    **assignment operations** are legal if and only if:

    -   Both types are the same and the LHS is an lvalue - the result
        type is that of the LHS

    It is LEGAL to assign arrays of the same base type but different
    lengths regardless if the LHS or RHS is bigger

    The result type is that of the LHS operand in legal cases, ERROR
    otherwise.
    :::

-   **`read` operations** are legal if and only if:

    -   The operand is of `int`, `bool` or `array byte[η]`, for any
        lengt η. Additionally, the read operand must be an lvalue

-   **`write` operations** are legal if and only if:

    -   The operand is of type `int`
    -   The operand is of type `bool`
    -   The operand is of type `array byte[η]`, for any length η

-   **`indexing` operations** are legal if and only if:

    -   The operand is promotable to `int` (i.e. int or byte)

    The result type is the base type of the array in legal cases, and
    ERROR otherwise.

-   **function calls** are legal if and only if:

    -   The identifier is of function type (i.e., the callee is actually
        a function).
    -   The number of actuals must match the number of formals.
    -   The type of each actual must match the type of the corresponding
        formal.

    If the identifier is not a function name, the result type is ERROR.
    Otherwise, it is the return type of the function *even if the
    arguments are ill-typed*.

-   **function returns**: The follow rules hold for function returns:

    -   A return statement in a `void` function may not have a value.
    -   A return statement in a non-`void` function must have a value.
    -   The return expression must match the function\'s declared type.

    It is LEGAL for a non-void function to skip a return statement. For
    example, this code is ok:

    `int f() {     //I should return an int, but I don't }`


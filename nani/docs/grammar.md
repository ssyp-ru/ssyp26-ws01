# Nani — Formal Grammar

What Nani has:
- numbers (written as integers or decimals, e.g. `4`, `3.14`)
- strings, booleans (`true`, `false`), and `nil`
- arithmetic, comparison, equality, and boolean operators
- variables declared with `let`, block scope, and assignment
- `if`, `while`, and `for`
- functions declared with `fn`; functions do **not** form closures
- maps created with the built-in `object()` function and accessed with value keys
- statements `print` and `assert`
- `//` line comments

## Lexical grammar

```
NUMBER          → DIGIT+ ( "." DIGIT+ )?
STRING          → '"' STRING_CHAR* '"'
IDENTIFIER      → ALPHA ( ALPHA | DIGIT | "_" )*
DIGIT           → "0" ... "9"
ALPHA           → "a" ... "z" | "A" ... "Z" | "_"
STRING_CHAR     → any character except '"' or a newline
```

Keywords: `and`, `assert`, `else`, `false`, `fn`, `for`, `if`, `let`,
`nil`, `or`, `print`, `return`, `true`, `while`.

```
"(" ")" "{" "}" "[" "]" "," ";"
"+" "-" "*" "/"
"!" "!="
"=" "=="
"<" "<=" ">" ">="
```

A comment starts with `//` and ends at the next newline. Whitespace and
comments separate tokens and otherwise have no meaning.

## Operator precedence

1: assignment: `=` (right-associative)
2: `or`
3: `and`
4: `== !=`
5: `< <= > >=`
6: `+ -` (binary)
7: `* /`
8: `! -` (unary)
9: calls and keyed access: `()` `[]`

## Syntax grammar

```
program        → declaration* EOF ;

declaration    → fnDecl
               | letDecl
               | statement ;

fnDecl         → "fn" function ;
function       → IDENTIFIER "(" parameters? ")" block ;
parameters     → IDENTIFIER ( "," IDENTIFIER )* ;

letDecl        → "let" IDENTIFIER ( "=" expression )? ";" ;

statement      → assertStmt
               | returnStmt
               | block
               | expressionStmt
               | forStmt
               | ifStmt
               | printStmt
               | whileStmt ;

assertStmt     → "assert" expression ";" ;
returnStmt     → "return" expression? ";" ;
block          → "{" declaration* "}" ;
expressionStmt → expression ";" ;
printStmt      → "print" expression ";" ;

ifStmt         → "if" "(" expression ")" statement
               | "if" "(" expression ")" statement "else" statement ;
whileStmt      → "while" "(" expression ")" statement ;
forStmt        → "for" "(" ( letDecl | expressionStmt | ";" )
                 expression? ";" expression? ")" statement ;

expression     → assignment ;
assignment     → logic_or ( "=" assignment )? ;

logic_or       → logic_and ( "or" logic_and )* ;
logic_and      → equality  ( "and" equality )* ;

equality       → comparison ( ( "==" | "!=" ) comparison )* ;
comparison     → term       ( ( "<" | "<=" | ">" | ">=" ) term )* ;
term           → factor     ( ( "+" | "-" ) factor )* ;
factor         → unary      ( ( "*" | "/" ) unary )* ;

unary          → ( "!" | "-" ) unary
               | call ;

call           → primary ( "(" arguments? ")"
                         | "[" expression "]" )* ;
arguments      → expression ( "," expression )* ;

primary        → "false"
               | "true"
               | "nil"
               | NUMBER
               | STRING
               | IDENTIFIER
               | "(" expression ")" ;
```

The left-hand side of an assignment must be an identifier or keyed access.
`object()` creates an empty map. A map key is any non-`nil` value; reading a
missing map key evaluates to `nil`.

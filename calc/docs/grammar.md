# Calc — Formal Grammar

What calc has:
- floating point numbers (written as integers or decimals, e.g. `4`, `3.14`)
- booleans (`true`, `false`)
- arithmetic operators (`+ - * /`, unary `-`)
- comparison operators (`== != < > <= >=`)
- boolean operators (`! && ||`)
- exactly two statement kinds: `print` and `assert`
- `//` line comments

## Lexical grammar

```
NUMBER     → DIGIT+ ( "." DIGIT+ )?
DIGIT      → "0" ... "9"
```

Keywords: `true`, `false`, `print`, `assert`.


```
"(" ")" ";"
"+" "-" "*" "/"
"!" "!="
"==" 
"<" "<=" ">" ">="
"&&" "||"
```

## Operator precedence

1: ||
2: &&
3: == !=
4: < <= > >=
5: + - (binary)
6: * /
7: ! - (unary)


## Syntax grammar


```
program        → statement* EOF ;

statement      → printStmt
               | assertStmt ;

printStmt      → "print" expression ";" ;
assertStmt     → "assert" expression ";" ;

expression     → logic_or ;

logic_or       → logic_and ( "||" logic_and )* ;
logic_and      → equality  ( "&&" equality )* ;

equality       → comparison ( ( "==" | "!=" ) comparison )* ;
comparison     → term       ( ( "<" | "<=" | ">" | ">=" ) term )* ;

term           → factor ( ( "+" | "-" ) factor )* ;
factor         → unary  ( ( "*" | "/" ) unary )* ;

unary          → ( "!" | "-" ) unary
               | primary ;

primary        → NUMBER
               | "true"
               | "false"
               | "(" expression ")" ;
```

## Example of wrong grammar
```
expression     → literal
               | unary
               | binary
               | grouping ;

literal        → NUMBER | STRING | "true" | "false" | "nil" ;
grouping       → "(" expression ")" ;
unary          → ( "-" | "!" ) expression ;
binary         → expression operator expression ;
operator       → "==" | "!=" | "<" | "<=" | ">" | ">="
               | "+"  | "-"  | "*" | "/" ;
```

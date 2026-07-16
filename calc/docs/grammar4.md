```text
NUMBER     → DIGIT+ ( "." DIGIT+ )?
DIGIT      → "0" ... "9"

program    → statement* EOF ;

statement  → printStmt
           | assertStmt ;

printStmt  → "print" expression ";" ;
assertStmt → "assert" expression ";" ;

expression → logic_or ;

logic_or   → logic_and ( "||" logic_and )* ;
logic_and  → equality ( "&&" equality )* ;

equality   → comparison ( ( "==" | "!=" ) comparison )* ;
comparison → term ( ( "<" | "<=" | ">" | ">=" ) term )* ;

term       → factor ( ( "+" | "-" ) factor )* ;
factor     → unary ( ( "*" | "/" ) unary )* ;
unary      → ( "!" | "-" ) unary
           | primary ;
primary    → NUMBER
           | "true"
           | "false"
           | "(" expression ")" ;
```

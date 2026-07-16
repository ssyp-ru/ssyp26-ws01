```text
NUMBER     → DIGIT+ ( "." DIGIT+ )?
DIGIT      → "0" ... "9"

program    → statement* EOF ;

statement  → printStmt
           | assertStmt ;

printStmt  → "print" expression ";" ;
assertStmt → "assert" expression ";" ;

expression → term ;
term       → factor ( ( "+" | "-" ) factor )* ;
factor     → unary ( ( "*" | "/" ) unary )* ;
unary      → "-" unary
           | primary ;
primary    → NUMBER
           | "(" expression ")" ;
```

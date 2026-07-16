```text
NUMBER     → DIGIT+ ( "." DIGIT+ )?
DIGIT      → "0" ... "9"

program    → expression EOF ;

expression → term ;
term       → factor ( ( "+" | "-" ) factor )* ;
factor     → unary ( ( "*" | "/" ) unary )* ;
unary      → "-" unary
           | NUMBER ;
```

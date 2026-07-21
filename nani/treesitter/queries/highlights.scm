; https://tree-sitter.github.io/tree-sitter/3-syntax-highlighting.html
; in nvim type :help treesitter-highlight-groups

(comment) @comment
(number) @number

["true" "false"] @boolean
"nil" @constant.builtin

[
  "fn"
  "let"
  "if"
  "else"
  "while"
  "for"
  "print"
  "assert"
  "return"
] @keyword

["and" "or" "!" "=" "==" "!=" "<" "<=" ">" ">=" "+" "-" "*" "/"] @operator

["(" ")" "{" "}" "[" "]"] @punctuation.bracket
["," ";"] @punctuation.delimiter

; More-specific identifier captures below override this default capture.
(identifier) @variable

(function
  (identifier) @function)

(parameters
  (identifier) @variable.parameter)

(letDecl
  (identifier) @variable)

(call
  (primary
    (identifier) @function.call)
  (arguments))

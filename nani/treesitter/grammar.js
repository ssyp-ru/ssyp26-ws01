// Docs: https://tree-sitter.github.io/tree-sitter/creating-parsers/4-writing-the-grammar.html

module.exports = grammar({
  name: "nani",

  extras: ($) => [/\s/, $.comment],

  rules: {
    source_file: ($) => repeat($.declaration),
    declaration: ($) => choice($.fnDecl, $.letDecl, $.statement),

    fnDecl: ($) => seq("fn", $.function),
    function: ($) => seq($.identifier, "(", optional($.parameters), $.block),
    parameters: ($) => seq($.identifier, repeat(seq(",", $.identifier))),

    letDecl: ($) => seq("let", $.identifier, optional(seq("=", $.expression)), ";"),

    statement: ($) => choice(
        $.assertStmt,
        $.returnStmt,
        $.block,
        $.expressionStmt,
        $.forStmt,
        $.ifStmt,
        $.printStmt,
        $.whileStmt),

    assertStmt: ($) => seq("assert", $.expression, ";"),
    returnStmt: ($) => seq("return", optional($.expression), ";"),
    block: ($) => seq("{", repeat($.declaration), "}"),
    expressionStmt: ($) => seq($.expression, ";"),
    printStmt: ($) => seq("print", $.expression, ";"),
    ifStmt : ($) => choice(
            seq("if", "(", $.expression, ")", $.block), 
            seq("if", "(", $.expression, ")", $.block, "else", $.block)),
    whileStmt: ($) => seq("while", "(", $.expression, ")", $.statement),
    forStmt: ($) => seq("for", "(", choice($.letDecl, $.expressionStmt, ";" ), optional($.expression), ";", optional($.expression), ")", $.statement),
    
    expression: ($) => $.assignment,
    assignment: ($) => seq($.logic_or, optional(seq("=", $.assignment))),
    
    logic_or: ($) => seq($.logic_and, repeat(seq("or", $.logic_and))),
    logic_and: ($) => seq($.equality, repeat(seq("and", $.equality))),
    
    equality: ($) => seq($.comparison, repeat(seq(choice( "==", "!="), $.comparison ))), 
    comparison: ($) => seq($.term,  repeat(seq( choice("<", "<=", ">", ">="), $.term ))),
    
    term: ($) => seq($.factor, repeat(seq(choice("+", "-"), $.factor))),
    factor: ($) => seq($.unary, repeat(seq(choice("*", "/"), $.unary))),

    unary: ($) => choice(seq(choice("!", "-"), $.unary), $.call),

    call: ($) => seq($.primary, repeat(choice(seq("(", optional($.arguments), ")"), seq("[", $.expression, "]")))),
    
    arguments: ($) => seq($.expression, repeat(seq(",", $.expression))),

    primary: ($) => choice("false", "true", "nil", $.string, $.number, $.identifier, seq("(", $.expression, ")")),

    comment: ($) => token(seq("//", /[^\n]*/)),

    identifier: ($) => /[A-Za-z_][A-Za-z0-9]*/,
    number: $ => /\d+(\.\d+)?/,
    string: $ => /"[^"\n]*"/,
  },
});

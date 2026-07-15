// Docs: https://tree-sitter.github.io/tree-sitter/creating-parsers/3-writing-the-grammar.html

module.exports = grammar({
  name: "nani",

  extras: ($) => [/\s/, $.comment],

  rules: {
    source_file: ($) => repeat(choice($.comment, $.expression)),

    comment: ($) => token(seq("//", /[^\n]*/)),
    expression: ($) => seq($.number, $.plus, $.number),

    number: $ => /\d+(\.\d+)?/,
    plus: $ => "+",
    // string: $ => /"[^"\n]*"/,
  },
});

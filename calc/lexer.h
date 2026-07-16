#ifndef CALC_LEXER_H
#define CALC_LEXER_H

typedef enum {
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_SEMICOLON,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,

    TOKEN_BANG,
    TOKEN_BANG_EQ,
    TOKEN_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQ,
    TOKEN_GREATER,
    TOKEN_GREATER_EQ,
    TOKEN_AND,
    TOKEN_OR,

    TOKEN_NUMBER,

    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_PRINT,
    TOKEN_ASSERT,

    // Special token type showing that we have lexer error in this string.
    // Token list with such token cannot be passed to parser,
    // but still can be used for something like syntax highlight.
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    int start;
    int end;
    int line;
} token_t;

typedef struct {
    token_t *data;
    int size;
    int capacity;
    bool had_error;
} token_list_t;

token_list_t tokenizer(const char *source);

#endif

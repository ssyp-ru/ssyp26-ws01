#ifndef NANI_CORE_LEXER_H
#define NANI_CORE_LEXER_H

#include "common.h"

typedef enum {
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_SEMICOLON,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SLASH,
    TOKEN_STAR,

    TOKEN_BANG,
    TOKEN_BANG_EQ,
    TOKEN_EQ,
    TOKEN_EQ_EQ,
    TOKEN_GREATER,
    TOKEN_GREATER_EQ,
    TOKEN_LESS,
    TOKEN_LESS_EQ,

    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,

    TOKEN_AND,
    TOKEN_ASSERT,
    TOKEN_ELSE,
    TOKEN_FALSE,
    TOKEN_FN,
    TOKEN_FOR,
    TOKEN_IF,
    TOKEN_LET,
    TOKEN_NIL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_STRUCT,
    TOKEN_THIS,
    TOKEN_TRUE,
    TOKEN_WHILE,
} token_kind_t;

typedef struct {
    token_kind_t type;
    const char *start;
    int length;
    int line;
} token_t;

typedef struct {
    token_t* ptr;
    int length;
    int capacity;
} tokens_t;

bool tokenize(tokens_t* tokens, const char* code);

#endif

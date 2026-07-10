#ifndef CALC_LEXER_H
#define CALC_LEXER_H

#include "common.h"

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
    TOKEN_ASSERT
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

typedef struct {
    const char *start;
    const char *current;
    int line;
    bool had_error;
} Lexer;

void lexer_init(Lexer *lexer, const char *source);
bool lexer_is_at_end(const Lexer *lexer);
bool lexer_next_token(Lexer *lexer, Token *out);
const char *token_type_name(TokenType type);

typedef struct {
    Token *items;
    int count;
    int capacity;
    bool had_error;
} TokenList;

void token_list_init(TokenList *list);
void token_list_push(TokenList *list, Token token);
void token_list_free(TokenList *list);

TokenList lexer_scan_all(const char *source);

#endif

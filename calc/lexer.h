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
} token_t;

typedef struct {
    const char *start;
    const char *current;
    int line;
    bool had_error;
} lexer_t;

void lexer_init(lexer_t *lexer, const char *source);
bool lexer_is_at_end(const lexer_t *lexer);
bool lexer_next_token(lexer_t *lexer, token_t *out);
const char *token_type_name(TokenType type);

typedef struct {
    token_t *items;
    int count;
    int capacity;
    bool had_error;
} token_list_t;

void token_list_init(token_list_t *list);
void token_list_push(token_list_t *list, token_t token);
void token_list_free(token_list_t *list);

token_list_t lexer_scan_all(const char *source);

#endif

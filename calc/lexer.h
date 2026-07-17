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
} token_type;

typedef struct {
    token_type t;
    int start;
    int finish;
    int line;
} token_t;

typedef struct {
    token_t *data;
    int size;
    int capacity;
    int had_error;
} token_list_t;


typedef struct token_array {
	token_t *data;
	int size;
	int capacity;
} token_array_t;  

void token_array_init(token_array_t *a);
void token_array_destroy(token_array_t *a);
void token_array_push_back(token_array_t *a, token_t value);
token_t token_array_pop_back(token_array_t *a);

token_type getTokenType(char sym);
void tokenize(char *text, token_array_t *a);
void output(char *text, token_array_t *t);
void token_output(token_array_t *t);

#endif

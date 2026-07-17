#ifndef TOKEN_H
#define TOKEN_H
#include "lexer.h"

token_type getTokenType(char sym);

void tokenize(char *text, token_array_t *a);

void output(char *text, token_array_t *t);

void token_output(token_array_t *t);

#endif

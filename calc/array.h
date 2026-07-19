#ifndef ARRAY_H
#define ARRAY_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum token_type{    //OK
	TOKEN_NUMBER,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MULTIPLY,
	TOKEN_DIVIDE,
    TOKEN_PARENTHESISL,
    TOKEN_PARENTHESISR,
    TOKEN_LESS,
    TOKEN_MORE,
    TOKEN_EQUAL,
    TOKEN_OTHER,
    TOKEN_W
} token_type;

typedef struct token{   //OK
	enum token_type t;
	int start;
	int finish;
} token_t;

typedef struct array{   //OK
	token_t *data;
	int size;
	int capacity;
} array_t;

void init_array(array_t *a);
void destroy_array(array_t *a);
void push_array(array_t *a, token_t value);
token_t pop_array(array_t *a);

#endif

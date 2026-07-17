#pragma once

#include "lexer.h"
#include "value.h"

typedef enum {
	EXPR_LITERAL,
	EXPR_UNARY,
	EXPR_BINARY,
} expr_kind_t;

typedef struct expr_t expr_t;
struct expr_t {
	expr_kind_t kind;

	union {
		struct {
			value_t val;
		} literal;

		struct {
			token_kind_t op;
			expr_t* operand;
		} unary;

		struct {
			token_kind_t op;
			expr_t* l;
			expr_t* r;
		} binary;
	};
};

typedef enum {
	STMT_PRINT,
	STMT_ASSERT,
} stmt_kind_t;

typedef struct {
	stmt_kind_t kind;
	expr_t* expr;
} stmt_t;

typedef struct {
	stmt_t* ptr;
	int length;
	int capacity;
} program_t;

void free_ast(program_t* ast);
void parse_program(program_t* ast, tokens_t* tokens);


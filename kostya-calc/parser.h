#pragma once

#include "lexer.h"
#include "value.h"

typedef enum {
	EXPR_LITERAL,
	EXPR_UNARY,
	EXPR_BINARY,
} ExprKind;

typedef struct Expr Expr;
struct Expr {
	ExprKind kind;

	union {
		struct {
			Value val;
		} literal;

		struct {
			TokenKind op;
			Expr* operand;
		} unary;

		struct {
			TokenKind op;
			Expr* l;
			Expr* r;
		} binary;
	};
};

typedef enum {
	STMT_PRINT,
	STMT_ASSERT,
} StmtKind;

typedef struct {
	StmtKind kind;
	Expr* expr;
} Stmt;

typedef struct {
	Stmt* ptr;
	int length;
	int capacity;
} Program;

void free_ast(Program* ast);
void parse_program(Program* ast, Tokens* tokens);


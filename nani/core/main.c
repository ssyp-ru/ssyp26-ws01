#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lexer.h"
#include "parser.h"
#include "value.h"

static char* read_full(FILE* fp) {
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* output = malloc(size + 1);
	fread(output, size, 1, fp);
	output[size] = 0;

	return output;
}

[[noreturn]] static void rterr(const char* text) {
	printf("Runtime error: %s", text);
	exit(1);
}

/*
static value_t eval(expr_t* expr) {
	if (expr->type == EXPR_LITERAL) {
		return expr->value.literal;
	}

	if (expr->type == EXPR_UNARY) {
		value_t inner = eval(expr->unary.operand);

		if (expr->value.unary.op.type == TOKEN_MINUS) {
			if (inner.type != VAL_NUMBER) rterr("Expected number after unary minus");

			inner.val.number = -inner.val.number;
		} else if (expr->unary.op == TOKEN_BANG) {
			if (inner.type != VAL_BOOL) rterr("Expected bool after unary bang");

			inner.val.boolean = !inner.val.boolean;
		} else assert(false);

		return inner;
	}

	switch (expr->binary.op) {
		// TODO
		default:
			assert(false);
	}
}
*/

static const char* token_name(token_kind_t kind) {
#define A(k) if (kind == k) return #k;
	A(TOKEN_NUMBER)
	A(TOKEN_SEMICOLON)
	A(TOKEN_PLUS)
	A(TOKEN_STAR)
	A(TOKEN_SLASH)
	A(TOKEN_MINUS)
	A(TOKEN_LEFT_PAREN)
	A(TOKEN_RIGHT_PAREN)
	A(TOKEN_BANG)
	A(TOKEN_TRUE)
	A(TOKEN_FALSE)
	A(TOKEN_AND)
	A(TOKEN_OR)
	A(TOKEN_EQ_EQ)
	A(TOKEN_BANG_EQ)
	A(TOKEN_LESS)
	A(TOKEN_LESS_EQ)
	A(TOKEN_GREATER)
	A(TOKEN_GREATER_EQ)
	A(TOKEN_PRINT)
	A(TOKEN_ASSERT)

	assert(false);
#undef A
}

/*
static void print_expr(expr_t* expr) {
	if (expr->kind == EXPR_LITERAL) {
		if (expr->literal.val.type == VAL_NUMBER) printf("literal(%lf)", expr->literal.val.val.number);
		else printf("literal(%s)", expr->literal.val.val.boolean ? "true" : "false");

		return;
	}

	if (expr->kind == EXPR_UNARY) {
		printf("unary(%s, ", token_name(expr->unary.op));
		print_expr(expr->unary.operand);
		printf(")");

		return;
	}

	printf("binary(");
	print_expr(expr->binary.l);
	printf(", %s, ", token_name(expr->binary.op));
	print_expr(expr->binary.r);
	printf(")");
}
*/

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE* fp = fopen(argv[1], "r");
	char* code = read_full(fp);
	fclose(fp);

	tokens_t tokens = { 0 };
	if (!tokenize(&tokens, code)) return 1;

	stmt_list_t ast;
	parse_program(&ast, &tokens);
	free(code);

	//print_expr(ast.ptr[0].expr);

	return 0;
}

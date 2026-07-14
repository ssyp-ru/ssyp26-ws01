#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"
#include "value.h"

[[noreturn]] static void err(Tokens* tokens, int* pos, const char* text) {
	Token* tok = &tokens->ptr[*pos];

	printf("Parser error at token %d: %s\n", *pos, text);
	exit(1);
}

static Stmt* add_stmt(Program* ast) {
	if (!ast->ptr) {
		ast->length = 0;
		ast->capacity = 16;
		ast->ptr = malloc(ast->capacity * sizeof(Stmt));
	}

	if (ast->length == ast->capacity) {
		ast->capacity *= 2;
		ast->ptr = realloc(ast->ptr, ast->capacity * sizeof(Stmt));
	}

	return &ast->ptr[ast->length++];
}

void free_ast(Program* ast) {
	// TODO
}

static void parse_stmt(Stmt* stmt, Tokens* tokens, int* pos);
static Expr* parse_or(Tokens* tokens, int* pos);
static Expr* parse_and(Tokens* tokens, int* pos);
static Expr* parse_equality(Tokens* tokens, int* pos);
static Expr* parse_comparison(Tokens* tokens, int* pos);
static Expr* parse_term(Tokens* tokens, int* pos);
static Expr* parse_factor(Tokens* tokens, int* pos);
static Expr* parse_unary(Tokens* tokens, int* pos);
static Expr* parse_primary(Tokens* tokens, int* pos);

void parse_program(Program* ast, Tokens* tokens) {
	int pos = 0;

	Stmt* stmt = add_stmt(ast);
	parse_stmt(stmt, tokens, &pos);
}

void parse_stmt(Stmt* stmt, Tokens* tokens, int* pos) {
	Token* tok = &tokens->ptr[*pos];
	(*pos)++;

	if (tok->kind == ASSERT) stmt->kind = STMT_ASSERT;
	else if (tok->kind == PRINT) stmt->kind = STMT_PRINT;
	else err(tokens, pos, "Expected statement (assert or print)");
	
	stmt->expr = parse_or(tokens, pos);

	tok = &tokens->ptr[*pos];
	(*pos)++;
	if (tok->kind != SEMICOLON) err(tokens, pos, "Expected semicolon");
}

Expr* parse_or(Tokens* tokens, int* pos) {
	Expr* left = parse_and(tokens, pos);

	Token* tok = &tokens->ptr[*pos];
	if (tok->kind != OR) return left;
	(*pos)++;

	Expr* right = parse_and(tokens, pos);

	Expr* out = malloc(sizeof(Expr));
	out->kind = EXPR_BINARY;
	out->binary.op = OR;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

Expr* parse_and(Tokens* tokens, int* pos) {
	Expr* left = parse_equality(tokens, pos);

	Token* tok = &tokens->ptr[*pos];
	if (tok->kind != AND) return left;
	(*pos)++;

	Expr* right = parse_equality(tokens, pos);

	Expr* out = malloc(sizeof(Expr));
	out->kind = EXPR_BINARY;
	out->binary.op = AND;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

Expr* parse_equality(Tokens* tokens, int* pos) {
	Expr* left = parse_comparison(tokens, pos);

	Token* tok = &tokens->ptr[*pos];
	if (tok->kind != EQU && tok->kind != NEQ) return left;
	(*pos)++;

	Expr* right = parse_comparison(tokens, pos);

	Expr* out = malloc(sizeof(Expr));
	out->kind = EXPR_BINARY;
	out->binary.op = tok->kind;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

Expr* parse_comparison(Tokens* tokens, int* pos) {
	Expr* left = parse_term(tokens, pos);

	Token* tok = &tokens->ptr[*pos];
	if (tok->kind != LESS && tok->kind != LESS_EQ && tok->kind != GREATER && tok->kind != GREATER_EQ) return left;
	(*pos)++;

	Expr* right = parse_term(tokens, pos);

	Expr* out = malloc(sizeof(Expr));
	out->kind = EXPR_BINARY;
	out->binary.op = tok->kind;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

Expr* parse_term(Tokens* tokens, int* pos) {
	Expr* left = parse_factor(tokens, pos);

	Token* tok = &tokens->ptr[*pos];
	if (tok->kind != ADD && tok->kind != MINUS) return left;
	(*pos)++;

	Expr* right = parse_factor(tokens, pos);

	Expr* out = malloc(sizeof(Expr));
	out->kind = EXPR_BINARY;
	out->binary.op = tok->kind;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

Expr* parse_factor(Tokens* tokens, int* pos) {
	Expr* left = parse_unary(tokens, pos);

	Token* tok = &tokens->ptr[*pos];
	if (tok->kind != MUL && tok->kind != DIV) return left;
	(*pos)++;

	Expr* right = parse_unary(tokens, pos);

	Expr* out = malloc(sizeof(Expr));
	out->kind = EXPR_BINARY;
	out->binary.op = tok->kind;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

Expr* parse_unary(Tokens* tokens, int* pos) {
	Token* tok = &tokens->ptr[*pos];
	if (tok->kind != BANG && tok->kind != MINUS) return parse_primary(tokens, pos);
	(*pos)++;

	Expr* out = malloc(sizeof(Expr));
	out->kind = EXPR_UNARY;
	out->unary.op = tok->kind;
	out->unary.operand = parse_unary(tokens, pos);
	return out;
}

Expr* parse_primary(Tokens* tokens, int* pos) {
	Token* tok = &tokens->ptr[*pos];
	(*pos)++;

	if (tok->kind == PAR_OPEN) {
		Expr* expr = parse_or(tokens, pos);

		Token* tok2 = &tokens->ptr[*pos];
		(*pos)++;
		if (tok2->kind != PAR_CLOSE) err(tokens, pos, "Expected right parentheses");

		return expr;
	}

	Expr* out = malloc(sizeof(Expr));
	out->kind = EXPR_LITERAL;

	if (tok->kind == NUMBER) {
		char* endptr = (char*) &tokens->code[tok->start + tok->len];

		out->literal.val.kind = VAL_NUM;
		out->literal.val.num = strtod(&tokens->code[tok->start], &endptr);
		return out;
	}

	if (tok->kind == TRUE || tok->kind == FALSE) {
		out->literal.val.kind = VAL_BOOL;
		out->literal.val.boolean = tok->kind == TRUE;
		return out;
	}

	free(out);
	err(tokens, pos, "Expected primary (expr in parentheses, number, true or false)");
}


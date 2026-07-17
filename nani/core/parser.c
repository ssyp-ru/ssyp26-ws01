#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"
#include "value.h"

[[noreturn]] static void err(tokens_t* tokens, int* pos, const char* text) {
	token_t* tok = &tokens->ptr[*pos];

	printf("Parser error at token %d: %s\n", *pos, text);
	exit(1);
}

static stmt_t* add_stmt(program_t* ast) {
	if (!ast->ptr) {
		ast->length = 0;
		ast->capacity = 16;
		ast->ptr = malloc(ast->capacity * sizeof(stmt_t));
	}

	if (ast->length == ast->capacity) {
		ast->capacity *= 2;
		ast->ptr = realloc(ast->ptr, ast->capacity * sizeof(stmt_t));
	}

	return &ast->ptr[ast->length++];
}

void free_ast(program_t* ast) {
	// TODO
}

static void parse_stmt(stmt_t* stmt, tokens_t* tokens, int* pos);
static expr_t* parse_or(tokens_t* tokens, int* pos);
static expr_t* parse_and(tokens_t* tokens, int* pos);
static expr_t* parse_equality(tokens_t* tokens, int* pos);
static expr_t* parse_comparison(tokens_t* tokens, int* pos);
static expr_t* parse_term(tokens_t* tokens, int* pos);
static expr_t* parse_factor(tokens_t* tokens, int* pos);
static expr_t* parse_unary(tokens_t* tokens, int* pos);
static expr_t* parse_primary(tokens_t* tokens, int* pos);

void parse_program(program_t* ast, tokens_t* tokens) {
	int pos = 0;

	stmt_t* stmt = add_stmt(ast);
	parse_stmt(stmt, tokens, &pos);
}

void parse_stmt(stmt_t* stmt, tokens_t* tokens, int* pos) {
	token_t* tok = &tokens->ptr[*pos];
	(*pos)++;

	if (tok->kind == TOKEN_ASSERT) stmt->kind = STMT_ASSERT;
	else if (tok->kind == TOKEN_PRINT) stmt->kind = STMT_PRINT;
	else err(tokens, pos, "Expected statement (assert or print)");
	
	stmt->expr = parse_or(tokens, pos);

	tok = &tokens->ptr[*pos];
	(*pos)++;
	if (tok->kind != TOKEN_SEMICOLON) err(tokens, pos, "Expected semicolon");
}

expr_t* parse_or(tokens_t* tokens, int* pos) {
	expr_t* left = parse_and(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->kind != TOKEN_OR) return left;
	(*pos)++;

	expr_t* right = parse_and(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->kind = EXPR_BINARY;
	out->binary.op = TOKEN_OR;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

expr_t* parse_and(tokens_t* tokens, int* pos) {
	expr_t* left = parse_equality(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->kind != TOKEN_AND) return left;
	(*pos)++;

	expr_t* right = parse_equality(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->kind = EXPR_BINARY;
	out->binary.op = TOKEN_AND;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

expr_t* parse_equality(tokens_t* tokens, int* pos) {
	expr_t* left = parse_comparison(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->kind != TOKEN_EQ_EQ && tok->kind != TOKEN_BANG_EQ) return left;
	(*pos)++;

	expr_t* right = parse_comparison(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->kind = EXPR_BINARY;
	out->binary.op = tok->kind;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

expr_t* parse_comparison(tokens_t* tokens, int* pos) {
	expr_t* left = parse_term(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->kind != TOKEN_LESS && tok->kind != TOKEN_LESS_EQ && tok->kind != TOKEN_GREATER && tok->kind != TOKEN_GREATER_EQ) return left;
	(*pos)++;

	expr_t* right = parse_term(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->kind = EXPR_BINARY;
	out->binary.op = tok->kind;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

expr_t* parse_term(tokens_t* tokens, int* pos) {
	expr_t* left = parse_factor(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->kind != TOKEN_PLUS && tok->kind != TOKEN_MINUS) return left;
	(*pos)++;

	expr_t* right = parse_factor(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->kind = EXPR_BINARY;
	out->binary.op = tok->kind;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

expr_t* parse_factor(tokens_t* tokens, int* pos) {
	expr_t* left = parse_unary(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->kind != TOKEN_STAR && tok->kind != TOKEN_SLASH) return left;
	(*pos)++;

	expr_t* right = parse_unary(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->kind = EXPR_BINARY;
	out->binary.op = tok->kind;
	out->binary.l = left;
	out->binary.r = right;
	return out;
}

expr_t* parse_unary(tokens_t* tokens, int* pos) {
	token_t* tok = &tokens->ptr[*pos];
	if (tok->kind != TOKEN_BANG && tok->kind != TOKEN_MINUS) return parse_primary(tokens, pos);
	(*pos)++;

	expr_t* out = malloc(sizeof(expr_t));
	out->kind = EXPR_UNARY;
	out->unary.op = tok->kind;
	out->unary.operand = parse_unary(tokens, pos);
	return out;
}

expr_t* parse_primary(tokens_t* tokens, int* pos) {
	token_t* tok = &tokens->ptr[*pos];
	(*pos)++;

	if (tok->kind == TOKEN_LEFT_PAREN) {
		expr_t* expr = parse_or(tokens, pos);

		token_t* tok2 = &tokens->ptr[*pos];
		(*pos)++;
		if (tok2->kind != TOKEN_RIGHT_PAREN) err(tokens, pos, "Expected right parentheses");

		return expr;
	}

	expr_t* out = malloc(sizeof(expr_t));
	out->kind = EXPR_LITERAL;

	if (tok->kind == TOKEN_NUMBER) {
		char* endptr = (char*) &tokens->code[tok->start + tok->len];

		out->literal.val.kind = VAL_NUM;
		out->literal.val.num = strtod(&tokens->code[tok->start], &endptr);
		return out;
	}

	if (tok->kind == TOKEN_TRUE || tok->kind == TOKEN_FALSE) {
		out->literal.val.kind = VAL_BOOL;
		out->literal.val.boolean = tok->kind == TOKEN_TRUE;
		return out;
	}

	free(out);
	err(tokens, pos, "Expected primary (expr in parentheses, number, true or false)");
}


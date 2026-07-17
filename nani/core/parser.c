#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

[[noreturn]] static void err(tokens_t* tokens, int* pos, const char* text) {
	token_t* tok = &tokens->ptr[*pos];

	printf("Parser error at token %d: %s\n", *pos, text);
	exit(1);
}

static stmt_t* add_stmt(stmt_list_t* ast) {
	if (!ast->items) {
		ast->count = 0;
		ast->capacity = 16;
		ast->items = malloc(ast->capacity * sizeof(stmt_t*));
	}

	if (ast->count == ast->capacity) {
		ast->capacity *= 2;
		ast->items = realloc(ast->items, ast->capacity * sizeof(stmt_t*));
	}

    stmt_t* stmt = malloc(sizeof(stmt_t));
	ast->items[ast->count++] = stmt;
    return stmt;
}

void free_ast(stmt_list_t* ast) {
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

void parse_program(stmt_list_t* ast, tokens_t* tokens) {
	int pos = 0;

    // TODO: continue parsing statements until eof
	stmt_t* stmt = add_stmt(ast);
	parse_stmt(stmt, tokens, &pos);
}

void parse_stmt(stmt_t* stmt, tokens_t* tokens, int* pos) {
	token_t* tok = &tokens->ptr[*pos];
	(*pos)++;

	if (tok->type == TOKEN_ASSERT) stmt->type = STMT_ASSERT;
	else if (tok->type == TOKEN_PRINT) stmt->type = STMT_PRINT;
	else err(tokens, pos, "Expected statement (assert or print)");

    // hack
	stmt->as.print.expression = parse_or(tokens, pos);

	tok = &tokens->ptr[*pos];
	(*pos)++;
	if (tok->type != TOKEN_SEMICOLON) err(tokens, pos, "Expected semicolon");
}

expr_t* parse_or(tokens_t* tokens, int* pos) {
	expr_t* left = parse_and(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->type != TOKEN_OR) return left;
	(*pos)++;

	expr_t* right = parse_and(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->type = EXPR_LOGICAL;
	out->value.logical.op = *tok;
	out->value.logical.left = left;
	out->value.logical.right = right;
	return out;
}

expr_t* parse_and(tokens_t* tokens, int* pos) {
	expr_t* left = parse_equality(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->type != TOKEN_AND) return left;
	(*pos)++;

	expr_t* right = parse_equality(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->type = EXPR_LOGICAL;
	out->value.logical.op = *tok;
	out->value.logical.left = left;
	out->value.logical.right = right;
	return out;
}

expr_t* parse_equality(tokens_t* tokens, int* pos) {
	expr_t* left = parse_comparison(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->type != TOKEN_EQ && tok->type != TOKEN_BANG_EQ) return left;
	(*pos)++;

	expr_t* right = parse_comparison(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->type = EXPR_LOGICAL;
	out->value.logical.op = *tok;
	out->value.logical.left = left;
	out->value.logical.right = right;
	return out;
}

expr_t* parse_comparison(tokens_t* tokens, int* pos) {
	expr_t* left = parse_term(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->type != TOKEN_LESS && tok->type != TOKEN_LESS_EQ && tok->type != TOKEN_GREATER && tok->type != TOKEN_GREATER_EQ) return left;
	(*pos)++;

	expr_t* right = parse_term(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->type = EXPR_LOGICAL;
	out->value.logical.op = *tok;
	out->value.logical.left = left;
	out->value.logical.right = right;
	return out;
}

expr_t* parse_term(tokens_t* tokens, int* pos) {
	expr_t* left = parse_factor(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->type != TOKEN_PLUS && tok->type != TOKEN_MINUS) return left;
	(*pos)++;

	expr_t* right = parse_factor(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->type = EXPR_BINARY;
	out->value.binary.op = *tok;
	out->value.binary.left = left;
	out->value.binary.right = right;
	return out;
}

expr_t* parse_factor(tokens_t* tokens, int* pos) {
	expr_t* left = parse_unary(tokens, pos);

	token_t* tok = &tokens->ptr[*pos];
	if (tok->type != TOKEN_STAR && tok->type != TOKEN_SLASH) return left;
	(*pos)++;

	expr_t* right = parse_unary(tokens, pos);

	expr_t* out = malloc(sizeof(expr_t));
	out->type = EXPR_BINARY;
	out->value.binary.op = *tok;
	out->value.binary.left = left;
	out->value.binary.right = right;
	return out;
}

expr_t* parse_unary(tokens_t* tokens, int* pos) {
	token_t* tok = &tokens->ptr[*pos];
	if (tok->type != TOKEN_BANG && tok->type != TOKEN_MINUS) return parse_primary(tokens, pos);
	(*pos)++;

	expr_t* out = malloc(sizeof(expr_t));
	out->type = EXPR_UNARY;
	out->value.unary.op = *tok;
	out->value.unary.right = parse_unary(tokens, pos);
	return out;
}

expr_t* parse_primary(tokens_t* tokens, int* pos) {
	token_t* tok = &tokens->ptr[*pos];
	(*pos)++;

	if (tok->type == TOKEN_LEFT_PAREN) {
		expr_t* expr = parse_or(tokens, pos);

		token_t* tok2 = &tokens->ptr[*pos];
		(*pos)++;
		if (tok2->type != TOKEN_RIGHT_PAREN) err(tokens, pos, "Expected right parentheses");

		return expr;
	}

	expr_t* out = malloc(sizeof(expr_t));
	out->type = EXPR_LITERAL;
    out->value.literal.lexeme = *tok;

	if (tok->type == TOKEN_NUMBER) {
		out->value.literal.type = LITERAL_NUMBER;
	} else if (tok->type == TOKEN_TRUE || tok->type == TOKEN_FALSE) {
		out->value.literal.type = LITERAL_BOOL;
	} else {
        free(out);
        err(tokens, pos, "Expected primary (expr in parentheses, number, true or false)");
    }

	return out;
}

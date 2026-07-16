#ifndef CALC_PARSER_H
#define CALC_PARSER_H

#include "lexer.h"
#include "value.h"

typedef struct expr_t expr_t;

typedef enum {
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_LOGICAL
} ExprType;

struct expr_t {
    ExprType type;
    int line;

    union {
        struct {
            value_t value;
        } literal;

        struct {
            TokenType op;
            expr_t *operand;
        } unary;

        struct {
            TokenType op;
            expr_t *l;
            expr_t *r;
        } binary;

        struct {
            TokenType op;
            expr_t *l;
            expr_t *r;
        } logical;
    } val;
};

expr_t *make_literal_expr(value_t value, int line);
expr_t *make_unary_expr(TokenType op, expr_t *operand, int line);
expr_t *make_binary_expr(TokenType op, expr_t *left, expr_t *right, int line);
expr_t *make_logical_expr(TokenType op, expr_t *left, expr_t *right, int line);
void free_expr(expr_t *expr);

typedef enum {
    STMT_PRINT,
    STMT_ASSERT
} StmtType;

typedef struct {
    StmtType type;
    int line;
    expr_t *expression;
} stmt_t;

typedef struct {
    stmt_t *data;
    int size;
    int capacity;
} stmt_list_t;

void stmt_list_init(stmt_list_t *list);
void stmt_list_push(stmt_list_t *list, stmt_t stmt);
void stmt_list_free(stmt_list_t *list);

stmt_list_t parser_parse_program(token_list_t *tokens);

#endif

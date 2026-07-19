#ifndef CALC_PARSER_H
#define CALC_PARSER_H

//#include "lexer.h"
#include "value.h"
#include "array.h"
#include "token.h"

typedef struct expr_t expr_t;

typedef struct token_list{ // OK
    token_t *data;
    int size;
    int capacity;
//    bool had_error;
} token_list_t;

typedef struct lexer{ // OK
    token_list_t token_list;
    char *str;
    int current;
} lexer_t;

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
            token_type op;
            expr_t *operand;
        } unary;

        struct {
            token_type op;
            expr_t *l;
            expr_t *r;
        } binary;

        struct {
            token_type op;
            expr_t *l;
            expr_t *r;
        } logical;
    } val;
};

expr_t *make_literal(value_t value);
expr_t *make_unary(token_t next, expr_t *operand);
expr_t *make_binary(token_t next, expr_t *left, expr_t *right);
expr_t *make_logical(token_type op, expr_t *left, expr_t *right);
void free_expr(expr_t *expr);

expr_t *p_expr(lexer_t *list);
expr_t *p_term(lexer_t *list);
expr_t *p_factor(lexer_t *list);
expr_t *p_unary(lexer_t *list);

void print_ast(expr_t *expr);
int interpretator(expr_t *expr);

/*
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
*/

//void stmt_list_init(stmt_list_t *list);
//void stmt_list_push(stmt_list_t *list, stmt_t stmt);
//void stmt_list_free(stmt_list_t *list);

//stmt_list_t parser_parse_program(token_list_t *tokens);

#endif

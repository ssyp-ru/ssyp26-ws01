#ifndef CALC_AST_H
#define CALC_AST_H

#include "common.h"
#include "lexer.h"
#include "value.h"

typedef struct Expr Expr;

typedef enum {
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_LOGICAL
} ExprType;

struct Expr {
    ExprType type;
    int line;

    union {
        struct {
            Value value;
        } literal;

        struct {
            TokenType op;
            Expr *operand;
        } unary;

        struct {
            TokenType op;
            Expr *l;
            Expr *r;
        } binary;

        struct {
            TokenType op;
            Expr *l;
            Expr *r;
        } logical;
    } val;
};

Expr *make_literal_expr(Value value, int line);
Expr *make_unary_expr(TokenType op, Expr *operand, int line);
Expr *make_binary_expr(TokenType op, Expr *left, Expr *right, int line);
Expr *make_logical_expr(TokenType op, Expr *left, Expr *right, int line);
void free_expr(Expr *expr);

typedef enum {
    STMT_PRINT,
    STMT_ASSERT
} StmtType;

typedef struct {
    StmtType type;
    int line;
    Expr *expression;
} Stmt;

typedef struct {
    Stmt *items;
    int count;
    int capacity;
} StmtList;

void stmt_list_init(StmtList *list);
void stmt_list_push(StmtList *list, Stmt stmt);
void stmt_list_free(StmtList *list);

#endif

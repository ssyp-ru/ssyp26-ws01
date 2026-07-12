#ifndef NANI_CORE_AST_H
#define NANI_CORE_AST_H

#include "lexer.h"

typedef struct expr_t expr_t;
typedef struct stmt_t stmt_t;
typedef struct function_decl_t function_decl_t;

typedef struct {
    expr_t **items;
    int count;
    int capacity;
} expr_list_t;

typedef struct {
    stmt_t **items;
    int count;
    int capacity;
} stmt_list_t;

typedef struct {
    function_decl_t *items;
    int count;
    int capacity;
} function_list_t;

typedef enum {
    LITERAL_NIL,
    LITERAL_BOOL,
    LITERAL_NUMBER,
    LITERAL_STRING
} LiteralType;

typedef enum {
    EXPR_LITERAL,
    EXPR_GROUPING,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_LOGICAL,
    EXPR_VARIABLE,
    EXPR_ASSIGN,
    EXPR_CALL,
    EXPR_GET,
    EXPR_SET
} ExprType;

struct expr_t {
    ExprType type;
    int line;
    /* -1 means global. The resolver fills this for local name expressions. */
    int resolved_depth;

    union {
        struct {
            LiteralType type;
            token_t lexeme;
        } literal;
        struct {
            expr_t *expression;
        } grouping;
        struct {
            token_t op;
            expr_t *right;
        } unary;
        struct {
            expr_t *left;
            token_t op;
            expr_t *right;
        } binary;
        struct {
            expr_t *left;
            token_t op;
            expr_t *right;
        } logical;
        struct {
            token_t name;
        } variable;
        struct {
            token_t name;
            expr_t *value;
        } assign;
        struct {
            expr_t *callee;
            token_t paren;
            expr_list_t arguments;
        } call;
        struct {
            expr_t *object;
            token_t name;
        } get;
        struct {
            expr_t *object;
            token_t name;
            expr_t *value;
        } set;
    } value;
};

struct function_decl_t {
    token_t name;
    token_t *parameters;
    int parameter_count;
    int parameter_capacity;
    stmt_list_t body;
};

typedef enum {
    STMT_ASSERT,
    STMT_BLOCK,
    STMT_EXPRESSION,
    STMT_FOR,
    STMT_FUNCTION,
    STMT_IF,
    STMT_LET,
    STMT_PRINT,
    STMT_RETURN,
    STMT_STRUCT,
    STMT_WHILE
} StmtType;

struct stmt_t {
    StmtType type;
    int line;

    union {
        struct {
            expr_t *expression;
        } assert_stmt;
        struct {
            stmt_list_t declarations;
        } block;
        struct {
            expr_t *expression;
        } expression;
        struct {
            stmt_t *initializer;
            expr_t *condition;
            expr_t *increment;
            stmt_t *body;
        } for_stmt;
        struct {
            function_decl_t declaration;
        } function;
        struct {
            expr_t *condition;
            stmt_t *then_branch;
            stmt_t *else_branch;
        } if_stmt;
        struct {
            token_t name;
            expr_t *initializer;
        } let;
        struct {
            expr_t *expression;
        } print;
        struct {
            token_t keyword;
            expr_t *value;
        } return_stmt;
        struct {
            token_t name;
            function_list_t methods;
        } struct_decl;
        struct {
            expr_t *condition;
            stmt_t *body;
        } while_stmt;
    } as;
};

#endif

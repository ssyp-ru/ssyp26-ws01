#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "c_utils/utils.h"

static token_t* get_tok(tokens_t* tokens, int* pos) {
    if (*pos >= tokens->length) {
        // hack, but simpler than checking for NULL everywhere
        static token_t eof = {
            .type = TOKEN_ERROR,
            .start = "[ERROR]",
            .length = 7,
            .line = -1,
        };
        return &eof;
    };

    return &tokens->ptr[*pos];
}

static void err(tokens_t* tokens, int* pos, const char* text) {
    // err is called after pos has been incremented
    // so we have to decrement to get the actual
    // token where the error occured
    (*pos)--;

    token_t* tok = get_tok(tokens, pos);

    char* tokstr = (char*) malloc((tok->length + 1) * sizeof(char));
    memcpy(tokstr, tok->start, tok->length);
    tokstr[tok->length] = 0;

    log_error("Parser error at line %d (token \"%s\"): %s\n", tok->line, tokstr, text);

    free(tokstr);
    exit(1);
}

static stmt_t* add_stmt(stmt_list_t* ast) {
    if (!ast->items) {
        ast->count = 0;
        ast->capacity = 16;
        ast->items = (stmt_t**) malloc(ast->capacity * sizeof(stmt_t*));
    }

    if (ast->count == ast->capacity) {
        ast->capacity *= 2;
        ast->items = (stmt_t**) realloc(ast->items, ast->capacity * sizeof(stmt_t*));
    }

    stmt_t* stmt = (stmt_t*) malloc(sizeof(stmt_t));
    ast->items[ast->count++] = stmt;
    return stmt;
}

static expr_t* make_logical_expr(expr_t* left, token_t op, expr_t* right) {
    log_debug("create logical expr");
    log_token(" exrp token:", &op);
    expr_t* out = (expr_t*) malloc(sizeof(expr_t));
    out->type = EXPR_LOGICAL;
    out->value.logical.op = op;
    out->value.logical.left = left;
    out->value.logical.right = right;
    return out;
}

static expr_t* make_binary_expr(expr_t* left, token_t op, expr_t* right) {
    log_debug("create binary expr");
    log_token("  bin exrp token:", &op);
    expr_t* out = (expr_t*) malloc(sizeof(expr_t));
    out->type = EXPR_BINARY;
    out->value.binary.op = op;
    out->value.binary.left = left;
    out->value.binary.right = right;
    return out;
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
    log_debug("parse_program: pos=%d", pos);
    log_token("  cur token", get_tok(tokens, &pos));

    while (pos < tokens->length) {
        stmt_t* stmt = add_stmt(ast);
        parse_stmt(stmt, tokens, &pos);
    }
}

void parse_stmt(stmt_t* stmt, tokens_t* tokens, int* pos) {
    log_debug("parse_stmt: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    token_t* tok = get_tok(tokens, pos);
    (*pos)++;

    if (tok->type == TOKEN_ASSERT) stmt->type = STMT_ASSERT;
    else if (tok->type == TOKEN_PRINT) stmt->type = STMT_PRINT;
    else err(tokens, pos, "Expected statement (assert or print)");

    // hack
    stmt->as.print.expression = parse_or(tokens, pos);

    tok = get_tok(tokens, pos);
    (*pos)++;
    if (tok->type != TOKEN_SEMICOLON) err(tokens, pos, "Expected semicolon");
}

expr_t* parse_or(tokens_t* tokens, int* pos) {
    log_debug("parse_or: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_and(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_OR) return left;
    (*pos)++;

    expr_t* right = parse_and(tokens, pos);
    return make_logical_expr(left, *tok, right);
}

expr_t* parse_and(tokens_t* tokens, int* pos) {
    log_debug("parse_and: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_equality(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_AND) return left;
    (*pos)++;

    expr_t* right = parse_equality(tokens, pos);
    return make_logical_expr(left, *tok, right);
}

expr_t* parse_equality(tokens_t* tokens, int* pos) {
    log_debug("parse_equality: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_comparison(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_EQ_EQ && tok->type != TOKEN_BANG_EQ) return left;
    (*pos)++;

    expr_t* right = parse_comparison(tokens, pos);
    return make_logical_expr(left, *tok, right);
}

expr_t* parse_comparison(tokens_t* tokens, int* pos) {
    log_debug("parse_comparison: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_term(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    switch (tok->type) {
        case TOKEN_LESS:
        case TOKEN_LESS_EQ:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQ:
            break;
        default:
            return left;
    }
    (*pos)++;

    expr_t* right = parse_term(tokens, pos);
    return make_logical_expr(left, *tok, right);
}

expr_t* parse_term(tokens_t* tokens, int* pos) {
    log_debug("parse_term: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_factor(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_PLUS && tok->type != TOKEN_MINUS) return left;
    (*pos)++;

    expr_t* right = parse_factor(tokens, pos);
    return make_binary_expr(left, *tok, right);
}

expr_t* parse_factor(tokens_t* tokens, int* pos) {
    log_debug("parse_factor: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    expr_t* left = parse_unary(tokens, pos);

    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_STAR && tok->type != TOKEN_SLASH) return left;
    (*pos)++;

    expr_t* right = parse_unary(tokens, pos);
    return make_binary_expr(left, *tok, right);
}

expr_t* parse_unary(tokens_t* tokens, int* pos) {
    log_debug("parse_unary: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    token_t* tok = get_tok(tokens, pos);
    if (tok->type != TOKEN_BANG && tok->type != TOKEN_MINUS) return parse_primary(tokens, pos);
    (*pos)++;

    expr_t* out = (expr_t*) malloc(sizeof(expr_t));
    out->type = EXPR_UNARY;
    out->value.unary.op = *tok;
    out->value.unary.right = parse_unary(tokens, pos);
    return out;
}

expr_t* parse_primary(tokens_t* tokens, int* pos) {
    log_debug("parse_primary: pos=%d", *pos);
    log_token("  cur token", get_tok(tokens, pos));
    token_t* tok = get_tok(tokens, pos);
    (*pos)++;

    if (tok->type == TOKEN_LEFT_PAREN) {
        expr_t* expr = parse_or(tokens, pos);

        token_t* tok2 = get_tok(tokens, pos);
        (*pos)++;
        if (tok2->type != TOKEN_RIGHT_PAREN) err(tokens, pos, "Expected right parentheses");

        return expr;
    }

    expr_t* out = (expr_t*) malloc(sizeof(expr_t));
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

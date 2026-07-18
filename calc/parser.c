#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "value.h"

token_t get_token(lexer_t *lexer) {
    token_t t = lexer->tokens.data[lexer->cur];
    lexer->cur++;
    return t;
}

token_t peek_token(lexer_t *lexer) {
    return lexer->tokens.data[lexer->cur];
}

expr_t * p_term(lexer_t *lexer);
expr_t * p_factor(lexer_t *lexer);
expr_t * p_unary(lexer_t *lexer);

expr_t * p_expr(lexer_t *lexer) {
    return p_term(lexer);
}

expr_t * p_term(lexer_t *lexer) {
    expr_t * l = p_factor(lexer);

    token_t t = peek_token(lexer);
    if (t.t == TOKEN_PLUS || t.t == TOKEN_MINUS) {
        get_token(lexer); // get PLUS or MINUS
        expr_t * r = p_factor(lexer);
        expr_t * res = make_binary_expr(t.t, l, r);
        return res;
    }
    return l;
}

expr_t * p_factor(lexer_t *lexer) {
    expr_t * e = p_unary(lexer);
    return e;
}

expr_t * p_unary(lexer_t *lexer) {
    // TODO: actual number parsing
    token_t t = get_token(lexer);

    if (t.t == TOKEN_NUMBER) { 
        char *number_start = lexer->orig + t.start;
        float real_value = strtof(number_start, NULL);
        return make_literal_expr(real_value);
    }

    return make_literal_expr(1);
}

expr_t *make_literal_expr(float value) {
    expr_t *res = (expr_t*) malloc(sizeof(expr_t));
    res->type = EXPR_LITERAL;
    res->line = 0; // TODO
    res->val.literal.value.type = VAL_NUMBER;
    res->val.literal.value.val.number = value;
    return res;
}

expr_t *make_binary_expr(token_type op, expr_t *left, expr_t *right) {
    expr_t *res = (expr_t*) malloc(sizeof(expr_t));
    res->type = EXPR_BINARY;
    res->line = 0; // TODO
    res->val.binary.op = op;
    res->val.binary.l = left;
    res->val.binary.r = right;
    return res;
}

void print_ast(expr_t *expr) {
    switch(expr->type) {
        case EXPR_LITERAL:
            {
                value_t val = expr->val.literal.value;
                switch(val.type){
                    case VAL_NUMBER:
                        printf("literal %lf\n", val.val.number);
                        break;
                    case VAL_BOOL:
                        break;
                }
                break;
            }
        case EXPR_UNARY:
        break;
        case EXPR_BINARY:
            printf("type binary\n");
            printf("\tleft: ");
            print_ast(expr->val.binary.l);
            printf("\tright: ");
            print_ast(expr->val.binary.r);
            break;
        case EXPR_LOGICAL:
        break;
    };
}

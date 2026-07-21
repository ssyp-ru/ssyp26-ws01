#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include "parser.h"
#include "array.h"

/*
typedef struct token{ // OK
    TokenType type;
    int start;
    int end;
} token_t;
*/

/*
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
*/

//functions

token_t get_token(lexer_t *list){ // ?
    token_t t = (list->token_list.data)[list->current];
    list->current++;
    // error?
    return t;
}

token_t peek_token(lexer_t *list){ // ?
    // error?
    return (list->token_list.data)[(list->current)];
}

int digit(char c){ // ?
    return c - '0';
}

int to_int(char *s, int start, int finish){ // ?
    int r = 0;
    for(int i = start; i <= finish; i++){
        r *= 10;
        r += digit(s[i]);
    }
    return r;
}

int parse_number(lexer_t *list, token_t token){ // ?
    char *str = list->str;
    int l = token.start;
    int r = token.finish;
    return to_int(str, l, r);
}

value_t make_number_value(double n){ // ?
    value_t *value = (value_t *)malloc(sizeof(value_t));
    value->val.number = n; // ?
    return *value;
}

expr_t *make_literal_number(int n){ // ?
    expr_t *number = (expr_t *)malloc(sizeof(expr_t));
    number->type = EXPR_LITERAL;
    number->val.literal.value = make_number_value(n); // value_t
    return number;
}

expr_t *make_binary(token_t next, expr_t *l, expr_t *r){ // ?
    expr_t *binary = (expr_t *)malloc(sizeof(expr_t));
    binary->type = EXPR_BINARY;
    binary->val.binary.op = next.t; // TokenType
    binary->val.binary.l = l;
    binary->val.binary.r = r;
    return binary;
}

void print_ast(expr_t *expr){
//    printf("TEST\n");
    if(expr){
        switch(expr->type){
            case EXPR_LITERAL:
                printf("%lf ", expr->val.literal.value.val.number);
                break;
            case EXPR_BINARY:
            {
                token_type t = expr->val.binary.op;
                char c;
                switch(t){
                    case TOKEN_PLUS:
                        c = '+';
                        break;
                    case TOKEN_MINUS:
                        c = '-';
                        break;
                    case TOKEN_MULTIPLY:
                        c = '*';
                        break;
                    default:
                        c = ' ';
                }
                printf("%c ", c);
                print_ast(expr->val.binary.l);
                print_ast(expr->val.binary.r);
                break;
            }
            default:
                printf("not +-*\n");
        }
    }
    else{
        printf("n ");
    }
}

// parcer

/*
expr_t *p_expr(lexer_t *list);
expr_t *p_term(lexer_t *list);
expr_t *p_factor(lexer_t *list);
expr_t *p_unary(lexer_t *list);
*/

expr_t *p_expr(lexer_t *list){ // OK
//    printf("expr_OK\n");
    expr_t *left = 0;
    while(list->current < list->token_list.size){
        left = p_term(list, left);
    }
    return left;
}

expr_t *p_term(lexer_t *list, expr_t *left){ // ?
    expr_t *l = p_factor(list, 0);
    if(left){
        l = left;
    }
    token_t next = peek_token(list);
    if(next.t == TOKEN_PLUS || next.t == TOKEN_MINUS){
        get_token(list);
//        get_token(list); // ???
        expr_t *r = p_factor(list, 0);
//        printf("term_OK\n");
        return make_binary(next, l, r);
    }
    else{
//        printf("term_OK\n");
        return l;
    }
}

expr_t *p_factor(lexer_t *list, expr_t *left){ // ?
    expr_t *l = p_unary(list);
    if(left){
        l = left;
    }
    token_t next = peek_token(list);
    if(next.t == TOKEN_MULTIPLY || next.t == TOKEN_DIVIDE){
        get_token(list);
        expr_t *r = p_unary(list);
//        printf("factor_OK\n");
        return make_binary(next, l, r);
    }
    else{
//        printf("factor_OK\n");
        return l;
    }
}

expr_t *p_unary(lexer_t *list){ // ?
    token_t t = get_token(list);
    if(t.t == TOKEN_NUMBER){
        int n = parse_number(list, t);
//        printf("Number: %d\n", n);
        return make_literal_number(n);
    }
    printf("not_number\n");
    return 0;
}

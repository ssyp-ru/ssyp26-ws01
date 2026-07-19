//#include <stdio.h>
#include "array.h"
#include "parser.h"
#include "value.h"

int interpretator(expr_t *expr){
    switch(expr->type){
        case EXPR_BINARY:
        {
            int l = interpretator(expr->val.binary.l);
            int r = interpretator(expr->val.binary.r);
            switch(expr->val.binary.op){
                case TOKEN_PLUS:
                    return l + r;
                    break;
                case TOKEN_MINUS:
                    return l - r;
                    break;
                case TOKEN_MULTIPLY:
                    return l * r;
                    break;
                case TOKEN_DIVIDE:
                    return l / r;
                    break;
                default:
                    return 0;
                    break;
            }
            break;
        }
        case EXPR_LITERAL:
            return expr->val.literal.value.val.number;
            break;
        default:
            return 0;
            break;
    }
}

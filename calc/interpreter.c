#include "interpreter.h"

value_t evaluate(expr_t * ex) {
    value_t result;
    if (ex == 0) {
        result.type = VAL_NUMBER;
        result.val.number = 0.0f;
        return result;
    }

    switch (ex->type) {
        case EXPR_LITERAL:
            return ex->val.literal.value;

        case EXPR_BINARY: {
            value_t left_val = evaluate(ex->val.binary.l);
            value_t right_val = evaluate(ex->val.binary.r);

            if (left_val.type == VAL_NUMBER && right_val.type == VAL_NUMBER) {
                result.type = VAL_NUMBER;
                if (ex->val.binary.op == TOKEN_PLUS) {
                    result.val.number = left_val.val.number + right_val.val.number;
                } else if (ex->val.binary.op == TOKEN_MINUS) {
                    result.val.number = left_val.val.number - right_val.val.number;
                }
                return result;
            }
        }

        case EXPR_UNARY:
            break;

        case EXPR_LOGICAL:
            break;
    }

    result.type = VAL_NUMBER;
    result.val.number = 0.0f;
    return result;
}

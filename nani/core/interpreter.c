#include <assert.h>
#include <stdlib.h>
#include "interpreter.h"
#include "value.h"
#include "lexer.h"
#include "c_utils/utils.h"

[[noreturn]] static void rterr(const char* text) {
    log_error("Runtime error: %s\n", text);
    exit(1);
}

static value_t eval(expr_t* expr) {
    if (expr->type == EXPR_LITERAL) {
        value_t val;

        switch (expr->value.literal.type) {
        case LITERAL_NUMBER:
            {
                token_t *tok = &expr->value.literal.lexeme;
                char* end = (char*) tok->start + tok->length;

                val.type = VAL_NUMBER;
                val.val.number = strtod(tok->start, &end);
                break;
            }
        case LITERAL_BOOL:
            val.type = VAL_BOOL;
            val.val.boolean = expr->value.literal.lexeme.type == TOKEN_TRUE;
        default:
            rterr("TODO: literals");
        }

        return val;
    }

    if (expr->type == EXPR_UNARY) {
        value_t inner = eval(expr->value.unary.right);

        if (expr->value.unary.op.type == TOKEN_MINUS) {
            if (inner.type != VAL_NUMBER) rterr("Expected number after unary minus");

            inner.val.number = -inner.val.number;
        } else if (expr->value.unary.op.type == TOKEN_BANG) {
            if (inner.type != VAL_BOOL) rterr("Expected bool after unary bang");

            inner.val.boolean = !inner.val.boolean;
        } else assert(false);

        return inner;
    }

    if (expr->type == EXPR_BINARY) {
        double left = eval(expr->value.binary.left).val.number;
        double right = eval(expr->value.binary.right).val.number;

        switch (expr->value.binary.op.type) {
        case TOKEN_PLUS:
            left += right;
            break;
        case TOKEN_MINUS:
            left -= right;
            break;
        case TOKEN_STAR:
            left *= right;
            break;
        case TOKEN_SLASH:
            left /= right;
            break;
        default:
            assert(false);
        }

        value_t val;
        val.type = VAL_NUMBER;
        val.val.number = left;
        return val;
    }

    if (expr->type == EXPR_LOGICAL) {
        value_t left = eval(expr->value.logical.left);
        value_t right = eval(expr->value.logical.right);

        value_t val;
        val.type = VAL_BOOL;

        switch (expr->value.logical.op.type) {
        case TOKEN_EQ_EQ:
            val.val.boolean = val_equal(&left, &right);
            break;

        case TOKEN_BANG_EQ:
            val.val.boolean = !val_equal(&left, &right);
            break;

        case TOKEN_LESS:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) rterr("comparision with non-numbers");
            val.val.boolean = left.val.number < right.val.number;
            break;

        case TOKEN_LESS_EQ:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) rterr("comparision with non-numbers");
            val.val.boolean = left.val.number <= right.val.number;
            break;

        case TOKEN_GREATER:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) rterr("comparision with non-numbers");
            val.val.boolean = left.val.number > right.val.number;
            break;

        case TOKEN_GREATER_EQ:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) rterr("comparision with non-numbers");
            val.val.boolean = left.val.number >= right.val.number;
            break;

        default:
            assert(false);
        }

        return val;
    }

    rterr("TODO: other expr types");
}

void interpret(stmt_t* stmt) {
    switch (stmt->type) {
    case STMT_ASSERT:
        {
            value_t val = eval(stmt->as.assert_stmt.expression);
            if (val.type != VAL_BOOL) rterr("assert statement with non-bool value");
            if (!val.val.boolean) rterr("assertion failed");
            break;
        }
    default:
        assert(false);
    }
}


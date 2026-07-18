#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "value.h"
#include "lexer.h"
#include "c_utils/utils.h"

static char* linedup(const char* start) {
    const char* end = strchr(start, '\n');
    int len = end - start;

    char* output = (char*) malloc(len);
    memcpy(output, start, len);
    output[len] = 0;

    return output;
}

static const char* get_line(const char* code, int line) {
    if (line <= 0) {
        return "[Invalid line]";
    }

    int cur = 1;
    for (int i = 0; code[i]; i++) {
        if (cur == line) {
            const char* start = &code[i];
            return linedup(start);
        }

        if (code[i] == '\n') cur++;
    }

    return "[Invalid line]";
}

[[noreturn]] static void rterr(const char* code, int line, const char* text) {
    log_error("Runtime error on line %d: %s\n    %s", line, text, get_line(code, line));
    exit(1);
}

static value_t eval(const char* code, expr_t* expr) {
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
            rterr(code, expr->line, "TODO: literals");
        }

        return val;
    }

    if (expr->type == EXPR_UNARY) {
        value_t inner = eval(code, expr->value.unary.right);

        if (expr->value.unary.op.type == TOKEN_MINUS) {
            if (inner.type != VAL_NUMBER) rterr(code, expr->line, "Expected number after unary minus");

            inner.val.number = -inner.val.number;
        } else if (expr->value.unary.op.type == TOKEN_BANG) {
            if (inner.type != VAL_BOOL) rterr(code, expr->line, "Expected bool after unary bang");

            inner.val.boolean = !inner.val.boolean;
        } else assert(false);

        return inner;
    }

    if (expr->type == EXPR_BINARY) {
        double left = eval(code, expr->value.binary.left).val.number;
        double right = eval(code, expr->value.binary.right).val.number;

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
        value_t left = eval(code, expr->value.logical.left);
        value_t right = eval(code, expr->value.logical.right);

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
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) rterr(code, expr->line, "comparision with non-numbers");
            val.val.boolean = left.val.number < right.val.number;
            break;

        case TOKEN_LESS_EQ:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) rterr(code, expr->line, "comparision with non-numbers");
            val.val.boolean = left.val.number <= right.val.number;
            break;

        case TOKEN_GREATER:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) rterr(code, expr->line, "comparision with non-numbers");
            val.val.boolean = left.val.number > right.val.number;
            break;

        case TOKEN_GREATER_EQ:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) rterr(code, expr->line, "comparision with non-numbers");
            val.val.boolean = left.val.number >= right.val.number;
            break;

        default:
            assert(false);
        }

        return val;
    }

    rterr(code, expr->line, "TODO: other expr types");
}

void interpret(const char* code, stmt_t* stmt) {
    switch (stmt->type) {
    case STMT_ASSERT:
        {
            value_t val = eval(code, stmt->as.assert_stmt.expression);
            if (val.type != VAL_BOOL) rterr(code, stmt->line, "assert statement with non-bool value");
            if (!val.val.boolean) rterr(code, stmt->line, "assertion failed");
            break;
        }
    default:
        assert(false);
    }
}


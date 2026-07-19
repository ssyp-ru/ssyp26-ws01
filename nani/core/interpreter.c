#include "interpreter.h"
#include "value.h"
#include "lexer.h"

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct variable_t {
    char* name;
    int name_length;
    value_t value;
};

int find_global(interpreter_t* interpreter, token_t name) {
    for (int i = 0; i < interpreter->global_count; i++) {
        if (interpreter->globals[i].name_length == name.length &&
            !memcmp(interpreter->globals[i].name, name.start, name.length))
            return i;
    }

    return -1;
}

void define_global(interpreter_t* interpreter, token_t name, value_t value) {
    int index = find_global(interpreter, name);
    if (index >= 0) {
        interpreter->globals[index].value = value;
        return;
    }

    if (interpreter->global_count == interpreter->global_capacity) {
        interpreter->global_capacity = interpreter->global_capacity ? interpreter->global_capacity * 2 : 16;
        interpreter->globals =
            (variable_t*)realloc(interpreter->globals, interpreter->global_capacity * sizeof(variable_t));
    }

    char* global_name = (char*)malloc(name.length + 1);
    memcpy(global_name, name.start, name.length);
    global_name[name.length] = '\0';

    interpreter->globals[interpreter->global_count].name = global_name;
    interpreter->globals[interpreter->global_count].name_length = name.length;
    interpreter->globals[interpreter->global_count].value = value;
    interpreter->global_count++;
}

value_t get_global(interpreter_t* interpreter, int line, token_t name) {
    int index = find_global(interpreter, name);
    if (index < 0)
        rterr(interpreter->code, line, "Undefined variable");

    return interpreter->globals[index].value;
}

void assign_global(interpreter_t* interpreter, int line, token_t name, value_t value) {
    int index = find_global(interpreter, name);
    if (index < 0)
        rterr(interpreter->code, line, "Undefined variable");

    interpreter->globals[index].value = value;
}

static value_t eval(interpreter_t* interpreter, expr_t* expr) {
    if (expr->type == EXPR_LITERAL) {
        value_t val;

        switch (expr->value.literal.type) {
        case LITERAL_NUMBER: {
            token_t* tok = &expr->value.literal.lexeme;
            char* end = (char*)tok->start + tok->length;

            val.type = VAL_NUMBER;
            val.val.number = strtod(tok->start, &end);
            break;
        }
        case LITERAL_BOOL:
            val.type = VAL_BOOL;
            val.val.boolean = expr->value.literal.lexeme.type == TOKEN_TRUE;
        default:
            rterr(interpreter->code, expr->line, "TODO: literals");
        }

        return val;
    }

    if (expr->type == EXPR_VARIABLE)
        return get_global(interpreter, expr->line, expr->value.variable.name);

    if (expr->type == EXPR_ASSIGN) {
        value_t value = eval(interpreter, expr->value.assign.value);
        assign_global(interpreter, expr->line, expr->value.assign.name, value);
        return value;
    }

    if (expr->type == EXPR_UNARY) {
        value_t inner = eval(interpreter, expr->value.unary.right);

        if (expr->value.unary.op.type == TOKEN_MINUS) {
            if (inner.type != VAL_NUMBER)
                rterr(interpreter->code, expr->line, "Expected number after unary minus");

            inner.val.number = -inner.val.number;
        } else if (expr->value.unary.op.type == TOKEN_BANG) {
            if (inner.type != VAL_BOOL)
                rterr(interpreter->code, expr->line, "Expected bool after unary bang");

            inner.val.boolean = !inner.val.boolean;
        } else
            assert(false);

        return inner;
    }

    if (expr->type == EXPR_BINARY) {
        double left = eval(interpreter, expr->value.binary.left).val.number;
        double right = eval(interpreter, expr->value.binary.right).val.number;

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
        value_t left = eval(interpreter, expr->value.logical.left);
        value_t right = eval(interpreter, expr->value.logical.right);

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
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER)
                rterr(interpreter->code, expr->line, "comparision with non-numbers");
            val.val.boolean = left.val.number < right.val.number;
            break;

        case TOKEN_LESS_EQ:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER)
                rterr(interpreter->code, expr->line, "comparision with non-numbers");
            val.val.boolean = left.val.number <= right.val.number;
            break;

        case TOKEN_GREATER:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER)
                rterr(interpreter->code, expr->line, "comparision with non-numbers");
            val.val.boolean = left.val.number > right.val.number;
            break;

        case TOKEN_GREATER_EQ:
            if (left.type != VAL_NUMBER || right.type != VAL_NUMBER)
                rterr(interpreter->code, expr->line, "comparision with non-numbers");
            val.val.boolean = left.val.number >= right.val.number;
            break;

        default:
            assert(false);
        }

        return val;
    }

    rterr(interpreter->code, expr->line, "TODO: other expr types");
}

void interpret(interpreter_t* interpreter, stmt_t* stmt) {
    switch (stmt->type) {
    case STMT_LET: {
        value_t value;
        if (stmt->as.let.initializer)
            value = eval(interpreter, stmt->as.let.initializer);
        else
            value.type = VAL_NIL;
        define_global(interpreter, stmt->as.let.name, value);
        break;
    }
    case STMT_EXPRESSION:
        eval(interpreter, stmt->as.expression.expression);
        break;
    case STMT_ASSERT: {
        value_t val = eval(interpreter, stmt->as.assert_stmt.expression);
        if (val.type != VAL_BOOL)
            rterr(interpreter->code, stmt->line, "assert statement with non-bool value");
        if (!val.val.boolean)
            rterr(interpreter->code, stmt->line, "assertion failed");
        break;
    }
    default:
        assert(false);
    }
}

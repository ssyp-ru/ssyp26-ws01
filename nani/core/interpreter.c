#include "interpreter.h"
#include "parser.h"
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

typedef enum {
    FUNCTION_USER,
    FUNCTION_NATIVE,
} function_type_t;

struct function_t {
    char* name;
    int name_length;
    function_type_t type;
    int arity;
    union {
        function_decl_t declaration;
        native_function_t native;
    } as;
};

value_t eval(interpreter_t* interpreter, variable_list_t* locals, expr_t* expr);
int execute_statement(interpreter_t* interpreter, variable_list_t* locals, stmt_t* stmt, value_t* return_value);

int find_variable(variable_t* variables, int count, token_t name) {
    for (int i = 0; i < count; i++) {
        if (variables[i].name_length == name.length && !memcmp(variables[i].name, name.start, name.length))
            return i;
    }

    return -1;
}

void define_variable(variable_list_t* variables, token_t name, value_t value) {
    int index = find_variable(variables->variables, variables->count, name);
    if (index >= 0) {
        variables->variables[index].value = value;
        return;
    }

    if (variables->count == variables->capacity) {
        variables->capacity = variables->capacity ? variables->capacity * 2 : 16;
        variables->variables = (variable_t*)realloc(variables->variables, variables->capacity * sizeof(variable_t));
    }

    char* variable_name = (char*)malloc(name.length + 1);
    memcpy(variable_name, name.start, name.length);
    variable_name[name.length] = '\0';

    variables->variables[variables->count].name = variable_name;
    variables->variables[variables->count].name_length = name.length;
    variables->variables[variables->count].value = value;
    variables->count++;
}

void free_variables(variable_list_t* variables) {
    for (int i = 0; i < variables->count; i++) {
        variable_t* var = &variables->variables[i];
        free(var->name);
        free_value(&var->value);
    }

    free(variables->variables);
}

int find_global(interpreter_t* interpreter, token_t name) {
    return find_variable(interpreter->globals.variables, interpreter->globals.count, name);
}

void define_global(interpreter_t* interpreter, token_t name, value_t value) {
    define_variable(&interpreter->globals, name, value);
}

value_t get_global(interpreter_t* interpreter, int line, token_t name) {
    int index = find_global(interpreter, name);
    if (index < 0)
        rterr(interpreter->code, line, "Undefined variable");

    return interpreter->globals.variables[index].value;
}

void assign_global(interpreter_t* interpreter, int line, token_t name, value_t value) {
    int index = find_global(interpreter, name);
    if (index < 0)
        rterr(interpreter->code, line, "Undefined variable");

    interpreter->globals.variables[index].value = value;
}

value_t get_variable(interpreter_t* interpreter, variable_list_t* locals, int line, token_t name) {
    if (locals) {
        int index = find_variable(locals->variables, locals->count, name);
        if (index >= 0)
            return locals->variables[index].value;
    }

    return get_global(interpreter, line, name);
}

void assign_variable(interpreter_t* interpreter, variable_list_t* locals, int line, token_t name, value_t value) {
    if (locals) {
        int index = find_variable(locals->variables, locals->count, name);
        if (index >= 0) {
            locals->variables[index].value = value;
            return;
        }
    }

    assign_global(interpreter, line, name, value);
}

int find_function(interpreter_t* interpreter, token_t name) {
    for (int i = 0; i < interpreter->function_count; i++) {
        if (interpreter->functions[i].name_length == name.length &&
            !memcmp(interpreter->functions[i].name, name.start, name.length))
            return i;
    }

    return -1;
}

void define_function(interpreter_t* interpreter, function_decl_t declaration) {
    int index = find_function(interpreter, declaration.name);
    if (index < 0) {
        if (interpreter->function_count == interpreter->function_capacity) {
            interpreter->function_capacity = interpreter->function_capacity ? interpreter->function_capacity * 2 : 16;
            interpreter->functions =
                (function_t*)realloc(interpreter->functions, interpreter->function_capacity * sizeof(function_t));
        }

        char* function_name = (char*)malloc(declaration.name.length + 1);
        memcpy(function_name, declaration.name.start, declaration.name.length);
        function_name[declaration.name.length] = '\0';

        index = interpreter->function_count++;
        interpreter->functions[index].name = function_name;
        interpreter->functions[index].name_length = declaration.name.length;
    }

    function_t* function = &interpreter->functions[index];
    function->type = FUNCTION_USER;
    function->arity = declaration.parameter_count;
    function->as.declaration = declaration;
}

void define_native_function(interpreter_t* interpreter, const char* name, int arity, native_function_t native) {
    token_t token = {
        .type = TOKEN_IDENTIFIER,
        .start = name,
        .length = (int)strlen(name),
        .line = 0,
    };
    int index = find_function(interpreter, token);
    if (index < 0) {
        if (interpreter->function_count == interpreter->function_capacity) {
            interpreter->function_capacity = interpreter->function_capacity ? interpreter->function_capacity * 2 : 16;
            interpreter->functions =
                (function_t*)realloc(interpreter->functions, interpreter->function_capacity * sizeof(function_t));
        }

        char* function_name = (char*)malloc(token.length + 1);
        memcpy(function_name, name, token.length);
        function_name[token.length] = '\0';

        index = interpreter->function_count++;
        interpreter->functions[index].name = function_name;
        interpreter->functions[index].name_length = token.length;
    }

    function_t* function = &interpreter->functions[index];
    function->type = FUNCTION_NATIVE;
    function->arity = arity;
    function->as.native = native;
}

value_t call_function(interpreter_t* interpreter, variable_list_t* locals, expr_t* expr) {
    if (expr->value.call.callee->type != EXPR_VARIABLE)
        rterr(interpreter->code, expr->line, "Can only call named functions");

    token_t name = expr->value.call.callee->value.variable.name;
    int index = find_function(interpreter, name);
    if (index < 0)
        rterr(interpreter->code, expr->line, "Undefined function");

    function_t* function = &interpreter->functions[index];
    if (expr->value.call.arguments.count != function->arity)
        rterr(interpreter->code, expr->line, "Wrong number of arguments");

    value_t* arguments = (value_t*)malloc(function->arity * sizeof(value_t));
    for (int i = 0; i < function->arity; i++) {
        arguments[i] = eval(interpreter, locals, expr->value.call.arguments.items[i]);
    }

    if (function->type == FUNCTION_NATIVE) {
        value_t value = function->as.native(interpreter, expr->line, arguments, function->arity);
        free(arguments);
        return value;
    }

    function_decl_t* declaration = &function->as.declaration;
    variable_list_t function_locals = {0};
    for (int i = 0; i < declaration->parameter_count; i++) {
        define_variable(&function_locals, declaration->parameters[i], arguments[i]);
    }
    free(arguments);

    value_t return_value = nil_value();
    for (int i = 0; i < declaration->body.count; i++) {
        if (execute_statement(interpreter, &function_locals, declaration->body.items[i], &return_value))
            break;
    }

    free_variables(&function_locals);
    return return_value;
}

value_t eval(interpreter_t* interpreter, variable_list_t* locals, expr_t* expr) {
    if (expr->type == EXPR_LITERAL) {
        value_t val;
        token_t* tok = &expr->value.literal.lexeme;

        switch (expr->value.literal.type) {
        case LITERAL_NUMBER: {
            char* end = (char*)tok->start + tok->length;

            val.type = VAL_NUMBER;
            val.val.number = strtod(tok->start, &end);
            break;
        }
        case LITERAL_BOOL:
            val.type = VAL_BOOL;
            val.val.boolean = tok->type == TOKEN_TRUE;
            break;
        case LITERAL_STRING:
            val.type = VAL_STRING;
            string_t* str = (string_t*)malloc(sizeof(string_t));
            val.val.string = str;

            str->length = tok->length - 2;
            str->chars = (char*)malloc(str->length + 1);
            memcpy(str->chars, tok->start + 1, str->length);
            str->chars[str->length] = 0;
            break;
        /* TODO(OBJ): Evaluate LITERAL_NIL */
        default:
            rterr(interpreter->code, expr->line, "TODO: literals");
        }

        return val;
    }

    if (expr->type == EXPR_VARIABLE)
        return get_variable(interpreter, locals, expr->line, expr->value.variable.name);

    if (expr->type == EXPR_ASSIGN) {
        value_t value = eval(interpreter, locals, expr->value.assign.value);
        assign_variable(interpreter, locals, expr->line, expr->value.assign.name, value);
        return value;
    }

    /*
     * TODO(OBJ): Нужно обработать EXPR_KEY, для этого нужно evaluate сам объект и его ключ.
     после этого находим по ключю значение и возвращаем (nil если не нашли)
     Примерно тоже самое для EXPR_KEY_SET, только присваиваем его
     */

    if (expr->type == EXPR_CALL)
        return call_function(interpreter, locals, expr);

    if (expr->type == EXPR_UNARY) {
        value_t inner = eval(interpreter, locals, expr->value.unary.right);

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
        double left = eval(interpreter, locals, expr->value.binary.left).val.number;
        double right = eval(interpreter, locals, expr->value.binary.right).val.number;

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
        value_t left = eval(interpreter, locals, expr->value.logical.left);
        value_t right = eval(interpreter, locals, expr->value.logical.right);

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

        free_value(&left);
        free_value(&right);

        return val;
    }

    rterr(interpreter->code, expr->line, "TODO: other expr types");
}

int execute_statement(interpreter_t* interpreter, variable_list_t* locals, stmt_t* stmt, value_t* return_value) {
    switch (stmt->type) {
    case STMT_LET: {
        value_t value;
        if (stmt->as.let.initializer)
            value = eval(interpreter, locals, stmt->as.let.initializer);
        else
            value = nil_value();

        if (locals)
            define_variable(locals, stmt->as.let.name, value);
        else
            define_global(interpreter, stmt->as.let.name, value);
        return 0;
    }
    case STMT_EXPRESSION:
        eval(interpreter, locals, stmt->as.expression.expression);
        return 0;
    case STMT_PRINT: {
        value_t val = eval(interpreter, locals, stmt->as.print.expression);
        print_value(&val);
        free_value(&val);
        return 0;
    }
    case STMT_ASSERT: {
        value_t val = eval(interpreter, locals, stmt->as.assert_stmt.expression);
        if (val.type != VAL_BOOL)
            rterr(interpreter->code, stmt->line, "assert statement with non-bool value");
        if (!val.val.boolean)
            rterr(interpreter->code, stmt->line, "assertion failed");
        return 0;
    }
    case STMT_FUNCTION:
        define_function(interpreter, stmt->as.function.declaration);
        return 0;
    case STMT_RETURN:
        if (!locals)
            rterr(interpreter->code, stmt->line, "Return outside function");
        if (stmt->as.return_stmt.value)
            *return_value = eval(interpreter, locals, stmt->as.return_stmt.value);
        else
            *return_value = nil_value();
        return 1;
    case STMT_IF: {
        value_t cond = eval(interpreter, locals, stmt->as.if_stmt.condition);
        if (cond.type != VAL_BOOL)
            rterr(interpreter->code, stmt->line, "non-bool expression in if condition");

        if (cond.val.boolean) {
            execute_statement(interpreter, locals, stmt->as.if_stmt.then_branch, return_value);
        } else if (stmt->as.if_stmt.else_branch) {
            execute_statement(interpreter, locals, stmt->as.if_stmt.else_branch, return_value);
        }

        return 0;
    }
    case STMT_WHILE:
        while (true) {
            value_t cond = eval(interpreter, locals, stmt->as.while_stmt.condition);
            if (cond.type != VAL_BOOL)
                rterr(interpreter->code, stmt->line, "non-bool expression in while condition");

            if (!cond.val.boolean)
                break;

            execute_statement(interpreter, locals, stmt->as.while_stmt.body, return_value);
        }

        return 0;
    case STMT_FOR:
        execute_statement(interpreter, locals, stmt->as.for_stmt.initializer, return_value);

        while (true) {
            value_t cond = eval(interpreter, locals, stmt->as.for_stmt.condition);
            if (cond.type != VAL_BOOL)
                rterr(interpreter->code, stmt->line, "non-bool expression in while condition");

            if (!cond.val.boolean)
                break;

            execute_statement(interpreter, locals, stmt->as.for_stmt.body, return_value);
            eval(interpreter, locals, stmt->as.for_stmt.increment);
        }

        return 0;
    case STMT_BLOCK:
        for (int i = 0; i < stmt->as.block.declarations.count; i++) {
            stmt_t* s = stmt->as.block.declarations.items[i];
            execute_statement(interpreter, locals, s, return_value);
        }

        return 0;
    default:
        assert(false);
    }
}

void interpret(interpreter_t* interpreter, stmt_t* stmt) {
    value_t return_value;
    execute_statement(interpreter, NULL, stmt, &return_value);
}

void free_interpreter(interpreter_t* interpreter) {
    for (int i = 0; i < interpreter->function_count; i++) {
        free(interpreter->functions[i].name);
    }

    free(interpreter->functions);
    free_variables(&interpreter->globals);
}

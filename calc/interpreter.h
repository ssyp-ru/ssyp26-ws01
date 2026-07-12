#ifndef CALC_INTERPRETER_H
#define CALC_INTERPRETER_H

#include "ast.h"
#include "common.h"

typedef enum {
    INTERPRET_OK,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

InterpretResult interpret(const stmt_list_t *statements);

#endif

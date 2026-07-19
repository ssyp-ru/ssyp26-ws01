#ifndef NANI_CORE_INTERPRETER_H
#define NANI_CORE_INTERPRETER_H

#include "common.h"
#include "parser.h"

typedef struct variable_t variable_t;

typedef struct {
    const char* code;
    variable_t* globals;
    int global_count;
    int global_capacity;
} interpreter_t;

void interpret(interpreter_t* interpreter, stmt_t* stmt);

#endif

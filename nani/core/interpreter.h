#ifndef NANI_CORE_INTERPRETER_H
#define NANI_CORE_INTERPRETER_H

#include "common.h"
#include "parser.h"

typedef struct variable_t variable_t;
typedef struct function_t function_t;

typedef struct {
    variable_t* variables;
    int count;
    int capacity;
} variable_list_t;

typedef struct {
    const char* code;
    variable_list_t globals;
    function_t* functions;
    int function_count;
    int function_capacity;
} interpreter_t;

void interpret(interpreter_t* interpreter, stmt_t* stmt);

#endif

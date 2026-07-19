#ifndef NANI_CORE_INTERPRETER_H
#define NANI_CORE_INTERPRETER_H

#include "common.h"
#include "parser.h"
#include "value.h"

typedef struct variable_t variable_t;
typedef struct function_t function_t;
typedef struct interpreter_t interpreter_t;
typedef value_t (*native_function_t)(interpreter_t* interpreter, int line, value_t* arguments, int count);

typedef struct {
    variable_t* variables;
    int count;
    int capacity;
} variable_list_t;

struct interpreter_t {
    const char* code;
    variable_list_t globals;
    function_t* functions;
    int function_count;
    int function_capacity;
};

void define_native_function(interpreter_t* interpreter, const char* name, int arity, native_function_t function);
void interpret(interpreter_t* interpreter, stmt_t* stmt);

#endif

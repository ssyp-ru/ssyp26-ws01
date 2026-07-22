#ifndef NANI_CORE_VALUE_H
#define NANI_CORE_VALUE_H

#include "common.h"
#include <stdbool.h>

typedef struct {
    char* chars;
    int length;
} string_t;

typedef enum {
    VAL_NIL,
    VAL_BOOL,
    VAL_NUMBER,
    VAL_STRING,
    VAL_OBJ
} value_type_t;

typedef struct {
    value_type_t type;
    union {
        int boolean;
        double number;
        string_t* string;
        struct obj_t* object;
    } val;
} value_t;

typedef struct {
    value_t key;
    value_t value;
} map_entry_t;

typedef struct obj_t {
    map_entry_t* entries;
    int count;
    int capacity;
} obj_t;

value_t nil_value();
value_t number_value(float n);
bool val_equal(value_t* a, value_t* b);
void print_value(value_t* val);
void free_value(value_t* val);

#endif

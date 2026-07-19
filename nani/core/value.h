#ifndef NANI_CORE_VALUE_H
#define NANI_CORE_VALUE_H

#include "common.h"
#include <stdbool.h>

typedef struct obj_t obj_t;

typedef enum {
    VAL_NIL,
    VAL_BOOL,
    VAL_NUMBER,
    VAL_OBJ
} value_type_t;

typedef struct {
    value_type_t type;
    union {
        int boolean;
        double number;
        obj_t* object;
    } val;
} value_t;

bool val_equal(value_t* a, value_t* b);

#endif

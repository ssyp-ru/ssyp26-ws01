#ifndef CALC_VALUE_H
#define CALC_VALUE_H

#include "common.h"

typedef enum {
    VAL_NUMBER,
    VAL_BOOL
} ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        bool boolean;
    } val;
} value_t;

value_t make_number_value(double number);
value_t make_bool_value(bool boolean);

bool value_is_number(value_t value);
bool value_is_bool(value_t value);

#endif

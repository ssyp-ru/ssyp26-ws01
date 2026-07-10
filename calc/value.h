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
} Value;

Value make_number_value(double number);
Value make_bool_value(bool boolean);

bool value_is_number(Value value);
bool value_is_bool(Value value);

double value_as_number(Value value);
bool value_as_bool(Value value);

const char *value_type_name(Value value);
void print_value(Value value);
bool values_equal(Value a, Value b);

#endif

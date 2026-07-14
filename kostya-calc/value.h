#pragma once

#include <stdbool.h>

typedef enum {
	VAL_NUM,
	VAL_BOOL,
} ValueKind;

typedef struct {
	ValueKind kind;

	union {
		double num;
		bool boolean;
	};
} Value;


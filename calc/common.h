#ifndef CALC_COMMON_H
#define CALC_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void report_error(int line, const char *where, const char *message);
void report_runtime_error(int line, const char *message);

#endif

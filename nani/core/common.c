#include "c_utils/utils.h"

#include <stdlib.h>
#include <string.h>

static char* linedup(const char* start) {
    const char* end = strchr(start, '\n');
    int len = end - start;

    char* output = (char*)malloc(len);
    memcpy(output, start, len);
    output[len] = 0;

    return output;
}

static const char* get_line(const char* code, int line) {
    if (line <= 0) {
        return "[Invalid line]";
    }

    int cur = 1;
    for (int i = 0; code[i]; i++) {
        if (cur == line) {
            const char* start = &code[i];
            return linedup(start);
        }

        if (code[i] == '\n')
            cur++;
    }

    return "[Invalid line]";
}

__attribute__((__noreturn__)) void rterr(const char* code, int line, const char* text) {
    log_error("Runtime error on line %d: %s\n    %s", line, text, get_line(code, line));
    exit(1);
}

#include "utils.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static log_level_t current_log_level = LOG_LEVEL_INFO;
static FILE *log_output;

static int log_level_is_valid(log_level_t level)
{
    return level >= LOG_LEVEL_DEBUG && level <= LOG_LEVEL_NONE;
}

static void invalid_log_level(const char *format, ...)
{
    va_list arguments;

    fputs("Invalid log level: ", stderr);
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fputc('\n', stderr);
    abort();
}

static void log_write(log_level_t level, const char *name, const char *format,
                      va_list arguments)
{
    FILE *output;

    if (level < current_log_level || format == NULL) {
        return;
    }

    output = log_output != NULL ? log_output : stdout;
    fprintf(output, "[%s] ", name);
    vfprintf(output, format, arguments);
    fputc('\n', output);
    fflush(output);
}

void log_set_level(log_level_t level)
{
    if (!log_level_is_valid(level)) {
        invalid_log_level("%d", (int)level);
    }

    current_log_level = level;
}

log_level_t log_get_level(void)
{
    return current_log_level;
}

void log_set_level_string(const char *level)
{
    if (level == NULL) {
        invalid_log_level("NULL");
    }

    if (strcmp(level, "debug") == 0) {
        log_set_level(LOG_LEVEL_DEBUG);
        return;
    }
    if (strcmp(level, "info") == 0) {
        log_set_level(LOG_LEVEL_INFO);
        return;
    }
    if (strcmp(level, "error") == 0) {
        log_set_level(LOG_LEVEL_ERROR);
        return;
    }
    if (strcmp(level, "none") == 0) {
        log_set_level(LOG_LEVEL_NONE);
        return;
    }

    invalid_log_level("%s", level);
}

int log_set_file(const char *filename)
{
    FILE *file;

    if (filename == NULL) {
        errno = EINVAL;
        return -1;
    }

    file = fopen(filename, "a");
    if (file == NULL) {
        return -1;
    }

    if (log_output != NULL) {
        fclose(log_output);
    }
    log_output = file;
    return 0;
}

void log_close_file(void)
{
    if (log_output != NULL) {
        fclose(log_output);
        log_output = NULL;
    }
}

void log_debug(const char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    log_write(LOG_LEVEL_DEBUG, "DEBUG", format, arguments);
    va_end(arguments);
}

void log_info(const char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    log_write(LOG_LEVEL_INFO, "INFO", format, arguments);
    va_end(arguments);
}

void log_error(const char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    log_write(LOG_LEVEL_ERROR, "ERROR", format, arguments);
    va_end(arguments);
}

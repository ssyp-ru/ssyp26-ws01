#include "native.h"

#include <raylib/src/raylib.h>

static double number_argument(interpreter_t* interpreter, int line, value_t* arguments, int index) {
    if (arguments[index].type != VAL_NUMBER)
        rterr(interpreter->code, line, "Expected number argument");

    return arguments[index].val.number;
}

static unsigned char color_argument(interpreter_t* interpreter, int line, value_t* arguments, int index) {
    double value = number_argument(interpreter, line, arguments, index);
    if (value < 0 || value > 255)
        rterr(interpreter->code, line, "Color argument must be between 0 and 255");

    return (unsigned char)value;
}

static Color color_arguments(interpreter_t* interpreter, int line, value_t* arguments, int index) {
    Color color;
    color.r = color_argument(interpreter, line, arguments, index);
    color.g = color_argument(interpreter, line, arguments, index + 1);
    color.b = color_argument(interpreter, line, arguments, index + 2);
    color.a = 255;
    return color;
}

static value_t native_init_window(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)count;
    int width = (int)number_argument(interpreter, line, arguments, 0);
    int height = (int)number_argument(interpreter, line, arguments, 1);
    if (width <= 0 || height <= 0)
        rterr(interpreter->code, line, "Window dimensions must be positive");

    InitWindow(width, height, "Nani");
    return nil_value();
}

static value_t native_window_should_close(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)interpreter;
    (void)line;
    (void)arguments;
    (void)count;

    value_t value;
    value.type = VAL_BOOL;
    value.val.boolean = WindowShouldClose();
    return value;
}

static value_t native_set_target_fps(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)count;
    SetTargetFPS((int)number_argument(interpreter, line, arguments, 0));
    return nil_value();
}

static value_t native_begin_drawing(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)interpreter;
    (void)line;
    (void)arguments;
    (void)count;
    BeginDrawing();
    return nil_value();
}

static value_t native_clear_background(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)count;
    ClearBackground(color_arguments(interpreter, line, arguments, 0));
    return nil_value();
}

static value_t native_draw_circle(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)count;
    int x = (int)number_argument(interpreter, line, arguments, 0);
    int y = (int)number_argument(interpreter, line, arguments, 1);
    float radius = (float)number_argument(interpreter, line, arguments, 2);
    DrawCircle(x, y, radius, color_arguments(interpreter, line, arguments, 3));
    return nil_value();
}

static value_t native_end_drawing(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)interpreter;
    (void)line;
    (void)arguments;
    (void)count;
    EndDrawing();
    return nil_value();
}

static value_t native_wait_time(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)count;
    WaitTime(number_argument(interpreter, line, arguments, 0));
    return nil_value();
}

static value_t native_close_window(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)interpreter;
    (void)line;
    (void)arguments;
    (void)count;
    CloseWindow();
    return nil_value();
}

void register_native_functions(interpreter_t* interpreter) {
    define_native_function(interpreter, "rl_init_window", 2, native_init_window);
    define_native_function(interpreter, "rl_window_should_close", 0, native_window_should_close);
    define_native_function(interpreter, "rl_set_target_fps", 1, native_set_target_fps);
    define_native_function(interpreter, "rl_begin_drawing", 0, native_begin_drawing);
    define_native_function(interpreter, "rl_clear_background", 3, native_clear_background);
    define_native_function(interpreter, "rl_draw_circle", 6, native_draw_circle);
    define_native_function(interpreter, "rl_end_drawing", 0, native_end_drawing);
    define_native_function(interpreter, "rl_wait_time", 1, native_wait_time);
    define_native_function(interpreter, "rl_close_window", 0, native_close_window);
}

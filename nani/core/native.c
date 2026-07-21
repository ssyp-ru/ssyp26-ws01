#include "native.h"

#include <raylib/src/raylib.h>

#include <stdio.h>
#include <stdlib.h>

static double number_argument(interpreter_t* interpreter, int line, value_t* arguments, int index) {
    if (arguments[index].type != VAL_NUMBER)
        rterr(interpreter->code, line, "Expected number argument");

    return arguments[index].val.number;
}

static char* string_argument(interpreter_t* interpreter, int line, value_t* arguments, int index) {
    if (arguments[index].type != VAL_STRING)
        rterr(interpreter->code, line, "Expected string argument");

    return arguments[index].val.string->chars;
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

static value_t native_draw_rectangle(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)count;
    int x = (int)number_argument(interpreter, line, arguments, 0);
    int y = (int)number_argument(interpreter, line, arguments, 1);
    float square_size_x = (float)number_argument(interpreter, line, arguments, 2);
    float square_size_y = (float)number_argument(interpreter, line, arguments, 3);
    DrawRectangle(x, y, square_size_x, square_size_y, color_arguments(interpreter, line, arguments, 4));
    return nil_value();
}

static value_t native_draw_line(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)count;
    int start_pos_x = (int)number_argument(interpreter, line, arguments, 0);
    int start_pos_y = (int)number_argument(interpreter, line, arguments, 1);
    int end_pos_x = (int)number_argument(interpreter, line, arguments, 2);
    int end_pos_y = (int)number_argument(interpreter, line, arguments, 3);
    DrawLine(start_pos_x, start_pos_y, end_pos_x, end_pos_y, color_arguments(interpreter, line, arguments, 4));
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

static value_t native_is_key_pressed(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    int key = (int)number_argument(interpreter, line, arguments, 0);

    value_t value;
    value.type = VAL_BOOL;
    value.val.boolean = IsKeyPressed(key);
    return value;
}

static value_t native_draw_text(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    char* text = (char*)string_argument(interpreter, line, arguments, 0);
    int posX = (int)number_argument(interpreter, line, arguments, 1);
    int posY = (int)number_argument(interpreter, line, arguments, 2);
    int fontSize = (int)number_argument(interpreter, line, arguments, 3);

    DrawText(text, posX, posY, fontSize, color_arguments(interpreter, line, arguments, 4));
    return nil_value();
}

static value_t native_readline(interpreter_t* interpreter, int line, value_t* arguments, int count) {
    (void)interpreter;
    (void)line;
    (void)arguments;
    (void)count;

    int character;
    while ((character = getchar()) != '\n' && character != EOF) {
    }

    return nil_value();
}

value_t native_object(interpreter_t* interpreter, int line, value_t* arguments, int count) { // ?
    value_t value;
    value.type = VAL_OBJ;
    value.val.object = (obj_t*)malloc(sizeof(obj_t));
    value.val.object->entries = (map_entry_t*)malloc(4 * sizeof(map_entry_t));
    value.val.object->count = 0;
    value.val.object->capacity = 4;
    return value;
}

// TODO(OBJ): добавляем функцию object(), которая возвращает value_t с пустым VAL_OBJ.
void register_native_functions(interpreter_t* interpreter) {
    define_native_function(interpreter, "rl_init_window", 2, native_init_window);
    define_native_function(interpreter, "rl_window_should_close", 0, native_window_should_close);
    define_native_function(interpreter, "rl_set_target_fps", 1, native_set_target_fps);
    define_native_function(interpreter, "rl_begin_drawing", 0, native_begin_drawing);
    define_native_function(interpreter, "rl_clear_background", 3, native_clear_background);
    define_native_function(interpreter, "rl_draw_circle", 6, native_draw_circle);
    define_native_function(interpreter, "rl_draw_rectangle", 7, native_draw_rectangle);
    define_native_function(interpreter, "rl_draw_line", 7, native_draw_line);
    define_native_function(interpreter, "rl_end_drawing", 0, native_end_drawing);
    define_native_function(interpreter, "rl_wait_time", 1, native_wait_time);
    define_native_function(interpreter, "rl_close_window", 0, native_close_window);
    define_native_function(interpreter, "rl_is_key_pressed", 1, native_is_key_pressed);
    define_native_function(interpreter, "rl_draw_text", 7, native_draw_text);
    define_native_function(interpreter, "readline", 0, native_readline);

    define_native_function(interpreter, "object", 0, native_object);
}

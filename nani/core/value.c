#include "value.h"
#include "hash.h"
#include "c_utils/utils.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

value_t nil_value() {
    value_t value;
    value.type = VAL_NIL;
    return value;
}

value_t number_value(float n) {
    value_t value;
    value.type = VAL_NUMBER;
    value.val.number = n;
    return value;
}

static int obj_find(obj_t* obj, value_t* key) {
    return has_key(obj->entries, *key);
}

static bool obj_equal(obj_t* a, obj_t* b) {
    if (a->count != b->count)
        return false;

    for (int i = 0; i < HASH_SIZE; i++) {
        for (node_t* node = a->entries->p[i]; node != NULL; node = node->next) {
            if (!obj_find(b, &node->value.key))
                return false;

            value_t value = get(b->entries, node->value.key);
            if (!val_equal(&node->value.value, &value))
                return false;
        }
    }

    return true;
}

bool val_equal(value_t* a, value_t* b) {
    if (a->type != b->type)
        return false;

    switch (a->type) {
    case VAL_NIL:
        return true;

    case VAL_BOOL:
        return a->val.boolean == b->val.boolean;

    case VAL_NUMBER:
        return a->val.number == b->val.number;

    case VAL_STRING:
        return a->val.string->length == b->val.string->length &&
               !memcmp(a->val.string->chars, b->val.string->chars, a->val.string->length);

    case VAL_OBJ:
        return obj_equal(a->val.object, b->val.object);
    }

    log_error("Unknown value type: %d", a->type);
    assert(0 && "internal error");
}

void print_value(value_t* val) {
    switch (val->type) {
    case VAL_NIL:
        printf("nil\n");
        break;

    case VAL_BOOL:
        printf(val->val.boolean ? "true\n" : "false\n");
        break;

    case VAL_NUMBER:
        printf("%lf\n", val->val.number);
        break;

    case VAL_STRING:
        printf("%s\n", val->val.string->chars);
        break;

    case VAL_OBJ:
        printf("print_object:\n");
        printf("count = %d\n", val->val.object->count);
        printf("capacity = %d\n", val->val.object->capacity);
        for (int i = 0; i < HASH_SIZE; i++) {
            for (node_t* node = val->val.object->entries->p[i]; node != NULL; node = node->next) {
                printf("key: ");
                print_value(&node->value.key);
                printf("value: ");
                print_value(&node->value.value);
                printf("\n");
            }
        }
        break;
    }
}

void free_value(value_t* val) {
    switch (val->type) {
    case VAL_STRING:
        free(val->val.string->chars);
        free(val->val.string);
        break;
    default:
        break;
    }
}

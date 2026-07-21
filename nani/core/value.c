#include "value.h"
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

static int obj_find(obj_t* obj, value_t* key) {
    for (int i = 0; i < obj->count; i++) {
        map_entry_t* entry = &obj->entries[i];

        if (val_equal(&entry->key, key))
            return i;
    }

    return -1;
}

static bool obj_equal(obj_t* a, obj_t* b) {
    if (a->count != b->count)
        return false;

    for (int i = 0; i < a->count; i++) {
        map_entry_t* entry_a = &a->entries[i];

        int idx_b = obj_find(b, &entry_a->key);
        if (idx_b == -1)
            return false;

        if (!val_equal(&entry_a->value, &b->entries[idx_b].value))
            return false;
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
        // TODO(OBJ):
        printf("print_object:\n");
        printf("count = %d\n", val->val.object->count);
        printf("capacity = %d\n", val->val.object->capacity);
        for (int i = 0; i < val->val.object->count; i++) {
            printf("key: ");
            print_value(&val->val.object->entries[i].key);
            printf("value: ");
            print_value(&val->val.object->entries[i].value);
            printf("\n");
        }
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

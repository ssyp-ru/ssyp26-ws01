#include "value.h"
#include "c_utils/utils.h"

#include <assert.h>
#include <string.h>

value_t nil_value() {
    value_t value;
    value.type = VAL_NIL;
    return value;
}

static bool obj_equal(obj_t* a, obj_t* b) {
    // TODO(OBJ):
    assert(false);
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

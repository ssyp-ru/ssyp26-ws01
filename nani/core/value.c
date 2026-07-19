#include "value.h"
#include "c_utils/utils.h"

#include <assert.h>

static bool obj_equal(obj_t* a, obj_t* b) {
    // TODO
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

    case VAL_OBJ:
        return obj_equal(a->val.object, a->val.object);
    }

    log_error("Unknown value type: %d", a->type);
    assert(0 && "internal error");
}

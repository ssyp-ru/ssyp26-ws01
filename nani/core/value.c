#include "value.h"
#include <assert.h>

bool val_equal(value_t* a, value_t* b) {
    if (a->type != b->type) return false;

    switch (a->type) {
    case VAL_NIL:
        return true;

    case VAL_BOOL:
        return a->val.boolean == b->val.boolean;

    case VAL_NUMBER:
        return a->val.number == b->val.number;

    default:
        assert(false);
    }
}


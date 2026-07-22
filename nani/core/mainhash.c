#include "value.h"
#include "hash.h"
#include "assert.h"

int main() {
    value_t val;
    val.type = VAL_BOOL;
    vars_t list;
    create_varlist(&list);
    map_entry_t b;
    b.key = number_value(1);
    b.value = val;
    put(&list, b);
    printf("%d\n", get(&list, b.key).type == VAL_BOOL);
    // remove_var(&list, b.key);
    // get(&list, b.key);
    destroy_varlist(&list);
}

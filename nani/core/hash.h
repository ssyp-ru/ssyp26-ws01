#ifndef NANI_CORE_HASH_H
#define NANI_CORE_HASH_H

#include "value.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 26

typedef struct node {
    map_entry_t value;
    struct node* next;
} node_t;

typedef struct vars_t {
    node_t** p;
    int count;
} vars_t;

void put(vars_t* a, map_entry_t b);
value_t get(vars_t* a, value_t key);
bool has_key(vars_t* a, value_t key);
void create_varlist(vars_t* a);
void remove_var(vars_t* a, value_t key);
void destroy_varlist(vars_t* a);

#endif

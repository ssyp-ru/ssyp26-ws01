#include "hash.h"

#include <string.h>

static unsigned hash_bytes(const void* bytes, size_t length) {
    const unsigned char* data = bytes;
    unsigned hash = 5381;

    for (size_t i = 0; i < length; i++)
        hash = hash * 33 + data[i];

    return hash;
}

int find_hash(value_t key) {
    unsigned hash = hash_bytes(&key.type, sizeof(key.type));

    switch (key.type) {
    case VAL_NIL:
        break;
    case VAL_BOOL:
        hash += hash_bytes(&key.val.boolean, sizeof(key.val.boolean));
        break;
    case VAL_NUMBER: {
        double number = key.val.number == 0.0 ? 0.0 : key.val.number;
        hash += hash_bytes(&number, sizeof(number));
        break;
    }
    case VAL_STRING:
        hash += hash_bytes(key.val.string->chars, key.val.string->length);
        break;
    case VAL_OBJ:
        /* Objects compare structurally and can change, so keep them in bucket 0. */
        return 0;
    }

    return (int)(hash % HASH_SIZE);
}

void destroy_nodes(node_t* head) {
    while (head != NULL) {
        node_t* next = head->next;
        free(head);
        head = next;
    }
}

node_t* insert_node(node_t* head, node_t* p, map_entry_t value_) {
    node_t* next_ = malloc(sizeof(node_t));
    next_->value = value_;
    if (p == NULL) {
        next_->next = head;
        return next_;
    }

    next_->next = p->next;
    p->next = next_;
    return head;
}

node_t* delete_node(node_t* head, node_t* p) {
    if (head == NULL || p == NULL)
        return head;
    if (head == p) {
        node_t* next_ = head->next;
        free(head);
        return next_;
    }

    node_t* previous = head;
    while (previous->next != NULL && previous->next != p)
        previous = previous->next;
    if (previous->next == p) {
        previous->next = p->next;
        free(p);
    }
    return head;
}

node_t* find_node(node_t* head, value_t key) {
    while (head != NULL) {
        if (val_equal(&head->value.key, &key))
            return head;
        head = head->next;
    }
    return NULL;
}

void create_varlist(vars_t* a) {
    a->p = calloc(HASH_SIZE, sizeof(node_t*));
    a->count = 0;
}

void remove_var(vars_t* a, value_t key) {
    int x = find_hash(key);
    node_t* node = find_node(a->p[x], key);
    if (node != NULL) {
        a->p[x] = delete_node(a->p[x], node);
        a->count--;
    }
}

void put(vars_t* a, map_entry_t b) {
    int x = find_hash(b.key);
    node_t* node = find_node(a->p[x], b.key);
    if (node != NULL) {
        node->value.value = b.value;
        return;
    }

    a->p[x] = insert_node(a->p[x], NULL, b);
    a->count++;
}

bool has_key(vars_t* a, value_t key) {
    int x = find_hash(key);
    return find_node(a->p[x], key) != NULL;
}

value_t get(vars_t* a, value_t key) {
    int x = find_hash(key);
    node_t* node = find_node(a->p[x], key);
    if (node != NULL)
        return node->value.value;
    return nil_value();
}

void destroy_varlist(vars_t* a) {
    for (int i = 0; i < HASH_SIZE; i++)
        destroy_nodes(a->p[i]);
    free(a->p);
    a->p = NULL;
    a->count = 0;
}

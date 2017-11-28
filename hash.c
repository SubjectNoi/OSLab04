//
// Created by subjectnoi on 11/28/17.
//

#include "hash.h"

void hash_init(hash_t* t, int __size) {
    t->size = __size;
    t->list = (list_t*)malloc(sizeof(list_t) * __size);
}

void hash_insert(hash_t* t, unsigned int __val) {
    int idx = __val % t->size;
    list_insert(&(t->list[idx]), __val);
}

void* hash_lookup(hash_t* t, unsigned int __val) {
    int idx = __val % t->size;
    return list_lookup(&t->list[idx], __val);
}

void hash_delete(hash_t* t, unsigned int __val) {
    int idx = __val % t->size;
    list_delete(&t->list[idx], __val);
}

void hash_print(hash_t* t) {
    int i;
    for (i = 0; i < t->size; i++) {
        list_print(&(t->list[i]));
    }
}
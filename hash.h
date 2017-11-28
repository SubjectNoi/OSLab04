//
// Created by subjectnoi on 11/28/17.
//

#ifndef LOCK_HASH_H
#define LOCK_HASH_H

#include "list.h"

typedef struct __hash_t {
    list_t* list;
    int size;
} hash_t;

void  hash_init(hash_t*, int);
void  hash_insert(hash_t*, unsigned int);
void* hash_lookup(hash_t*, unsigned int);
void  hash_delete(hash_t*, unsigned int);
void  hash_print(hash_t*);

#endif //LOCK_HASH_H

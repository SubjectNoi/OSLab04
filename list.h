//
// Created by subjectnoi on 11/28/17.
//

#ifndef LOCK_LIST_H
#define LOCK_LIST_H

#include "lock.h"

typedef struct __node_t {
    int val;
    struct __node_t* next;
} node_t;

typedef struct __list_t {
    node_t *head;
#if TEST_LOCK == SPIN_LOCK
    spinlock_t      lock;
#elif TEST_LOCK == MUTEX
    mutex_t         lock;
#elif TEST_LOCK == TWO_PHASES
    two_phases_t    lock;
#else
    pthread_mutex_t lock;
#endif
} list_t;

void  list_init(list_t*);
void  list_insert(list_t*, unsigned int);
void  list_delete(list_t*, unsigned int);
void* list_lookup(list_t*, unsigned int);
void  list_print(list_t*);

#endif //LOCK_LIST_H

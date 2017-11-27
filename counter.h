//
// Created by subjectnoi on 11/27/17.
//

#ifndef LOCK_COUNTER_H
#define LOCK_COUNTER_H

#include "lock.h"

typedef struct __counter_t {
    int             val;
#if TEST_LOCK == SPIN_LOCK
    spinlock_t      lock;
#elif TEST_LOCK == MUTEX
    mutex_t         lock;
#elif TEST_LOCK == TWO_PHASES
    two_phases_t    lock;
#else
    pthread_mutex_t lock;
#endif
} counter_t;

void counter_init(counter_t*, int);
int counter_get_value(counter_t*);
void counter_increment(counter_t*);
void counter_decrement(counter_t*);

#endif //LOCK_COUNTER_H

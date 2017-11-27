//
// Created by subjectnoi on 11/27/17.
//

#include "counter.h"

void counter_init(counter_t* t, int __val) {
    t->val = __val;
    lock_init(&t->lock);
}

int counter_get_value(counter_t* t) {
    return t->val;
}

void counter_increment(counter_t* t) {
    lock_acquire(&t->lock);
    t->val++;
    lock_release(&t->lock);
}

void counter_decrement(counter_t* t) {
    lock_acquire(&t->lock);
    t->val--;
    lock_release(&t->lock);
}

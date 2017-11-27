//
// Created by subjectnoi on 11/25/17.
//

#include "lock.h"

void queue_init(queue_t *t, int len) {
    t->threads = (int*)malloc(sizeof(int) * len);
    t->size = len;
    t->front = 0;
    t->rear = 0;
}

void queue_add(queue_t *t, int val) {
    if (t->front == (t->rear + 1) % t->size) {
        perror("queue full\n");
    }
    else {
        t->threads[t->rear] = val;
        t->rear = (t->rear + 1) % t->size;
    }
    return;
}

int queue_remove(queue_t* t) {
    if (t->front == t->rear) {
        perror("queue empty\n");
        return -1;
    }
    else {
        int res;
        res = t->threads[t->front];
        t->front = (t->front + 1) % t->size;
        return res;
    }
}

void spinlock_init(spinlock_t *lock) {
    lock->tag = 0;
}

void spinlock_acquire(spinlock_t *lock) {
    while (xchg(&lock->tag, 1) == 1);
}

void spinlock_release(spinlock_t *lock) {
    lock->tag = 0;
}

void mutex_init(mutex_t* lock) {
    lock->tag = 0;
}

void mutex_acquire(mutex_t* lock) {
    if (!xchg(&lock->tag, 1)) return;
    else {
        sys_futex(&lock->tag, FUTEX_WAIT, 1, 0, 0, 0);
        mutex_acquire(lock);
    }
}

void mutex_release(mutex_t* lock) {
    xchg(&lock->tag, 0);
    sys_futex(&lock->tag, FUTEX_WAKE, 1, 0, 0, 0);
}

void two_phases_init(two_phases_t* lock) {
    lock->tag = 0;
    lock->guard = 0;
    lock->cnt = 0;
}

void two_phases_acquire(two_phases_t* lock) {
    int v;
    if (!xchg(&lock->tag, 1)) return;

    while (xchg(&lock->guard, 1));       // atomic_increment(mutex);
    lock->cnt++;
    lock->guard = 0;

Label0:
Label1:
    if (!xchg(&lock->tag, 1)) {
        while (xchg(&lock->guard, 1));  // atomic_decrement(mutex);
        lock->cnt--;
        lock->guard = 0;
        return;
    }

    v = lock->tag;                      // futex_wait(mutex, v)
    if (!v) goto Label1;
    sys_futex(&lock->tag, FUTEX_WAIT, v, 0, 0, 0);
    goto Label0;
}

void two_phases_release(two_phases_t* lock) {
    lock->tag = 0;
    if (lock->cnt) sys_futex(&lock->tag, FUTEX_WAKE, lock->cnt, 0, 0, 0);
}

void lock_init(void* lock) {
#if TEST_LOCK == SPIN_LOCK
    spinlock_init((spinlock_t*)lock);
#elif TEST_LOCK == MUTEX
    mutex_init((mutex_t*)lock);
#elif TEST_LOCK == TWO_PHASES
    two_phases_init((two_phases_t*)lock);
#else
    pthread_mutex_init((pthread_mutex_t*)lock, NULL);
#endif
}

void lock_acquire(void* lock) {
#if TEST_LOCK == SPIN_LOCK
    spinlock_acquire((spinlock_t*)lock);
#elif TEST_LOCK == MUTEX
    mutex_acquire((mutex_t*)lock);
#elif TEST_LOCK == TWO_PHASES
    two_phases_acquire((two_phases_t*)lock);
#else
    pthread_mutex_lock((pthread_mutex_t*)lock);
#endif
}

void lock_release(void* lock) {
#if TEST_LOCK == SPIN_LOCK
    spinlock_release((spinlock_t*)lock);
#elif TEST_LOCK == MUTEX
    mutex_release((mutex_t*)lock);
#elif TEST_LOCK == TWO_PHASES
    two_phases_release((two_phases_t*)lock);
#else
    pthread_mutex_unlock((pthread_mutex_t*)lock);
#endif
}
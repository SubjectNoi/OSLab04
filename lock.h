//
// Created by subjectnoi on 11/25/17.
//

#ifndef LOCK_LOCK_H
#define LOCK_LOCK_H
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "xchg.c"
#include "sys_futex.c"

#define     SPIN_LOCK   0
#define     MUTEX       1
#define     TWO_PHASES  2
#define     PTHREAD     3

#define     TEST_LOCK   3 // 通过修改这个变量以切换main函数中测试的锁的类型，3是系统提供的锁pthread，main函数多线程使用pthread写

typedef struct __queue_t {
    int *threads;
    int front;
    int rear;
    int size;
} queue_t;
void queue_init(queue_t*, int);
void queue_add(queue_t*, int);
int queue_remove(queue_t*);

typedef struct __spinlock_t {
    int tag;
} spinlock_t;
void spinlock_init(spinlock_t*);
void spinlock_acquire(spinlock_t*);
void spinlock_release(spinlock_t*);

typedef struct __mutex_t {
    int tag;
} mutex_t;
void mutex_init(mutex_t*);
void mutex_acquire(mutex_t*);
void mutex_release(mutex_t*);

typedef struct __two_phases_t {
    int tag;
    int guard;
    int cnt;
} two_phases_t;
void two_phases_init(two_phases_t*);
void two_phases_acquire(two_phases_t*);
void two_phases_release(two_phases_t*);

void lock_init(void*);
void lock_acquire(void*);
void lock_release(void*);

#endif //LOCK_LOCK_H

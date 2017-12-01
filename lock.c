//
// Created by subjectnoi on 11/25/17.
//

#include "lock.h"
/*
 * 现无用，原先用于二相锁
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
*/

/*
 * 初始化自旋锁，tag置零代表无线程占用。
 */
void spinlock_init(spinlock_t *lock) {
    lock->tag = 0;
}

/*
 * 线程申请自旋锁，使用xchg原子语句[xchg($1, $2)含义为将$2的值赋给$1同时返回$1原来的值，具体可参照xchg.h中的定义]，若返回1(lock->tag = 1, 代表
 * 锁被占用)，则一直等待。
 */
void spinlock_acquire(spinlock_t *lock) {
    while (xchg(&lock->tag, 1) == 1);
}

/* 
 * 释放自旋锁，将tag置零代表无线程占用。
 */
void spinlock_release(spinlock_t *lock) {
    lock->tag = 0;
}

/*
 * 初始化mutex，tag置零代表无线程占用。
 */
void mutex_init(mutex_t* lock) {
    lock->tag = 0;
}

/*
 * 线程申请mutex，若xchg = 0(lock->tag = 0，锁未被占用)，则将lock->tag置1代表占用该锁。
 *               若xchg = 1(lock->tag = 1，锁已被占用)，与自旋锁不同（自旋锁while等待），此方法使用sys_futex阻塞该线程，不再占用CPU资源。
 * 注意，若线程0执行到sys_futex(...)这句被阻塞，下面这句mutex_acquire()暂时不会被执行，这个线程的PC连同寄存器都压栈，线程信息被切换为阻塞，
 * 下次这个线程被唤醒了直接从mutex_acquire()一句开始执行，尝试得到锁。
 * sys_mutex($1, $2, $3, ...)后面三个参数不用管，前三个参数的含义是: 当$2是FUTEX_WAIT时，往操作系统阻塞线程队列压入 被$1变量阻塞的$3个线程
 *                                                                当$2是FUTEX_WAKE时，从操作系统阻塞线程队列取出 被$1变量阻塞的$3个线程
 * 该函数中含义为：往操作系统阻塞队列压入一个（就是被阻塞的这个线程本身）被lock->tag（锁本身）阻塞的线程。
 */
void mutex_acquire(mutex_t* lock) {
    if (!xchg(&lock->tag, 1)) return;
    else {
        sys_futex(&lock->tag, FUTEX_WAIT, 1, 0, 0, 0);
        mutex_acquire(lock);
    }
}

/*
 * lock->tag置零，无线程占用，同时从操作系统阻塞队列中唤醒一个被锁阻塞的线程
 */
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

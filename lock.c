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

/*
 * 初始化二相锁，tag为锁本身，cnt为被tag阻塞的线程数，guard为cnt的锁，保证cnt同时只能被一个线程修改。
 */
void two_phases_init(two_phases_t* lock) {
    lock->tag = 0;
    lock->guard = 0;
    lock->cnt = 0;
}
/*
 * 获得二相锁，下面有具体步骤
 */
void two_phases_acquire(two_phases_t* lock) {
    int v;
    if (!xchg(&lock->tag, 1)) return;    // 若xchg = 0，则直接能获得锁tag，返回
                                         // 若不能直接获得锁，执行以下代码。
    while (xchg(&lock->guard, 1));       // 使用自旋锁取得cnt的锁guard，以保证同时只有一个线程能修改cnt
    lock->cnt++;                         // 因无法获得tag，该线程应该被标记为阻塞并被加入阻塞队列，被阻塞的线程cnt+1，其实写到这里就行了，但是
                                         // 为了性能，有了下面的代码，下面的代码是为了“看看这个线程还有没有的救”，如果有的救(突然有别的线程释放
                                         // 锁，tag可用，那么不用加入阻塞队列，这也是为什么这一块只有cnt++而没有sys_futex的原因)
    lock->guard = 0;                     // cnt修改完毕，释放guard

Label0:
Label1:
    if (!xchg(&lock->tag, 1)) {          // 突然有别的线程释放tag，那么很幸运，这个线程不用加进阻塞队列可以直接执行
        while (xchg(&lock->guard, 1));   // 通过自旋锁获得cnt的锁guard，以修改cnt(因为上面对cnt+1，而现在+1已经不必要了，所以要减回来)
        lock->cnt--;                     // cnt-1
        lock->guard = 0;                 // cnt修改完毕，释放guard
        return;                          // 该线程获得了tag，直接返回
    }

    v = lock->tag;                       // 最后再试试看能不能救回来
    if (!v) goto Label1;                 // 很幸运，又有别的线程释放tag，那么赶紧回到Label1获得锁
    sys_futex(&lock->tag, FUTEX_WAIT, v, 0, 0, 0); // 很不幸，是真的没有别的线程释放tag了，假如阻塞队列，这里v其实可以直接用1代替了
    goto Label0;
}

/*
 * 释放二相锁，先tag置零，同时尝试唤醒操作系统阻塞队列中被阻塞的所有线程，与mutex不同的是，mutex只唤醒一个，二相锁唤醒所有。
 */
void two_phases_release(two_phases_t* lock) {
    lock->tag = 0;
    if (lock->cnt) sys_futex(&lock->tag, FUTEX_WAKE, lock->cnt, 0, 0, 0);
}

/*
 * 预编译指令以泛用lock_init(), lock_acquire(), lock_release
 */
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

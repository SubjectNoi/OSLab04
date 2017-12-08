#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"
#include "list.h"
#include "hash.h"
#include <sys/time.h>

int thread_count;

counter_t cnt;
list_t lst;
hash_t hsh;

void* dosomething(void*);
void* doanything(void*);
void* donothing(void*);

int main(int argc, char* argv[]) {
    struct timeval st, ed;
    srand(time(NULL));
    counter_init(&cnt, 0);
    list_init(&lst);
    //hash_init(&hsh, 49999);

    pthread_t       *thread_handler;
    thread_handler = (pthread_t*)malloc(sizeof(pthread_t) * 1024);
    thread_count = strtol(argv[1], NULL, 10);
    long            thread;

    int i;
/*
    gettimeofday(&st, NULL);
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handler[thread], NULL, dosomething, (void *) thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handler[thread], NULL);
    }
    gettimeofday(&ed, NULL);
    printf("counter takes %d us.\n", (ed.tv_sec - st.tv_sec) * 1000000 + (ed.tv_usec - st.tv_usec));

*/
    gettimeofday(&st, NULL);
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handler[thread], NULL, doanything, (void *) thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handler[thread], NULL);
    }
    gettimeofday(&ed, NULL);
    printf("list takes %d us.\n", (ed.tv_sec - st.tv_sec) * 1000000 + (ed.tv_usec - st.tv_usec));

/*
    int bucket;
for (i = 0; i <= 11; i++) {
    switch (i) {
        case 0: bucket = 1; break;
        case 1: bucket = 49999; break;
        case 2: bucket = 27851; break;
        case 3: bucket = 12007; break;
        case 4: bucket = 6091; break;
        case 5: bucket = 3049; break;
        case 6: bucket = 1439; break;
        case 7: bucket =  727; break;
        case 8: bucket =  337; break;
        case 9: bucket =  163; break;
        case 10: bucket =   89; break;
        case 11: bucket =   43; break;
    }
    hash_init(&hsh, 49999);
    gettimeofday(&st, NULL);
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handler[thread], NULL, donothing, (void *) thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handler[thread], NULL);
    }
    gettimeofday(&ed, NULL);
    printf("%d hash takes %d us.\n", bucket, (ed.tv_sec - st.tv_sec) * 1000000 + (ed.tv_usec - st.tv_usec));
}
 */
    return 0;
}

void* dosomething(void* rank) {
    long my_rank = (long) rank;
    int i;
    for (i = 0; i < 1000000; i++) {
        counter_increment(&cnt);
    }
    for (i = 0; i < 1000000; i++) {
        counter_decrement(&cnt);
    }
}

void* doanything(void* rank) {
    long my_rank = (long) rank;
    int i;
    for (i = 0; i < 5000; i++) {
        list_insert(&lst, i);
    }
    for (i = 0; i < 5000; i++) {
        list_delete(&lst, i);
    }
}

void* donothing(void* rank) {
    long my_rank = (long) rank;
    int i;
    for (i = 0; i < 50000; i++) {
        hash_insert(&hsh, i);
    }
    for (i = 0; i < 50000; i++) {
        hash_delete(&hsh, i);
    }
}


#include <stdio.h>
#include <stdlib.h>
#include "counter.h"
#include "list.h"
#include "hash.h"

int thread_count;

counter_t cnt;
list_t lst;
hash_t hsh;

void* dosomething(void*);
void* doanything(void*);
void* donothing(void*);

int main(int argc, char* argv[]) {
    srand(time(NULL));
    counter_init(&cnt, 0);
    list_init(&lst);
    hash_init(&hsh, 117);

    pthread_t       *thread_handler;
    thread_handler = (pthread_t*)malloc(sizeof(pthread_t) * 1024);
    thread_count = strtol(argv[1], NULL, 10);
    long            thread;
    clock_t st, ed;
    int i, sum = 0;

    st = clock();
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handler[thread], NULL, donothing, (void *) thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handler[thread], NULL);
    }

    ed = clock();
    sum += (ed - st);

    printf("Takes %d cycles.\n", sum);


    return 0;
}

void* dosomething(void* rank) {
    long my_rank = (long) rank;
    int i;
    for (i = 0; i < 1000000; i++) {
        counter_increment(&cnt);
    }
}

void* doanything(void* rank) {
    long my_rank = (long) rank;
    int i;
    for (i = 0; i < 10000; i++) {
        list_insert(&lst, i);
    }
    for (i = 0; i < 5000; i++) {
        list_delete(&lst, random() % 10000);
    }
}

void* donothing(void* rank) {
    long my_rank = (long) rank;
    int i;
    for (i = 0; i < 2000; i++) {
        hash_insert(&hsh, i);
    }
    hash_print(&hsh);
    printf("------------------------------------------------------------------\n");
    for (i = 0; i < 1000; i++) {
        hash_delete(&hsh, random() % 2000);
    }
    hash_print(&hsh);
}


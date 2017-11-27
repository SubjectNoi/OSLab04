#include <stdio.h>
#include <stdlib.h>
#include "counter.h"

int thread_count;
counter_t cnt;
void* dosth(void* rank);

int main(int argc, char* argv[]) {
    counter_init(&cnt, 0);

    pthread_t       *thread_handler;
    thread_handler = (pthread_t*)malloc(sizeof(pthread_t) * 1024);
    thread_count = strtol(argv[1], NULL, 10);
    long            thread;
    clock_t st, ed;
    int i, sum = 0;
    for (i = 0; i < 10; i++) {
        st = clock();
        for (thread = 0; thread < thread_count; thread++) {
            pthread_create(&thread_handler[thread], NULL, dosth, (void *) thread);
        }

        for (thread = 0; thread < thread_count; thread++) {
            pthread_join(thread_handler[thread], NULL);
        }

        ed = clock();
        sum += (ed - st);
    }
    printf("counter is %d. Takes %f cycles.\n", counter_get_value(&cnt), sum / 10.0);


    return 0;
}

void* dosth(void* rank) {
    long my_rank = (long) rank;
    int i;
    for (i = 0; i < 1000000; i++) {
        counter_increment(&cnt);
    }
}
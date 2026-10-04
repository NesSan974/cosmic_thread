#ifndef __tp_h__
#define __tp_h__

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>

#include "nes_ds.h"


#define NB_THREAD 4

struct tp_arg;

typedef void (*tp_function_t)(struct tp_arg) ;

typedef struct tp_task {
    tp_function_t pfun;
    void* arg;
}tp_task;


typedef struct tp_worker {
    size_t id;
    pthread_t pthread;
    int status;
}tp_worker;

typedef struct tp_arg {
    tp_worker *worker;
    void* arg;
}tp_arg;

typedef struct tp_cb_task_queue {
    struct tp_task *items;

    size_t head;
    size_t tail;
    size_t count;
    size_t capacity;

}tp_cb_task_queue;

extern tp_cb_task_queue task_queue;
extern tp_worker thread_pool[NB_THREAD];

void tp_init(int nb_max_task);
void tp_submit(struct tp_task task);
void tp_waitall();

#endif // __tp_h__

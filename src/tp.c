#include "tp.h"


struct tp_cb_task_queue task_queue;
struct tp_worker thread_pool[NB_THREAD] = {0};

static pthread_mutex_t mutex_task_queue = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_task_available = PTHREAD_COND_INITIALIZER;


static void *thread_routine(void *arg) ;


void tp_init(int nb_max_task) {

    nesds_cbinit(task_queue, nb_max_task);

    if (!task_queue.items) {
        perror("malloc");
        abort();
    }

    for (size_t i = 0; i < NB_THREAD; i++) {
        thread_pool[i].id = i;
        pthread_create(&thread_pool[i].pthread, NULL, thread_routine, &thread_pool[i]);
    }
}

void tp_submit(struct tp_task task) {
    pthread_mutex_lock(&mutex_task_queue);

    cbenqueue(task_queue, task);

    pthread_cond_signal(&cond_task_available);
    pthread_mutex_unlock(&mutex_task_queue);
}


void tp_waitall() {

    for (size_t i = 0; i < NB_THREAD; i++) {
        pthread_join(thread_pool[i].pthread, 0);
    }
}

// ---

static void *thread_routine(void *arg) {

    tp_worker *w = (tp_worker *)arg;

    struct tp_task task = {0};

    while (1) {

        // Lock
        pthread_mutex_lock(&mutex_task_queue);
        while (task_queue.count == 0) {
            pthread_cond_wait(&cond_task_available, &mutex_task_queue);
        }

        // Pull from job queue
        nesds_cbdequeue(task_queue, task);

        // Unlock
        pthread_mutex_unlock(&mutex_task_queue);

        struct tp_arg tpa = {.worker=w, .arg=task.arg};

        task.pfun(tpa);
    }

    return NULL;
}

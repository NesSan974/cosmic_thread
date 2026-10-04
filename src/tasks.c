#include "tasks.h"
#include <assert.h>

enum mutex_ressources {
    MUT_RESS_DUST = 0x01 << RES_DUST,
    MUT_RESS_ROCK = 0x01 << RES_ROCK,
    MUT_RESS_METAL = 0x01 << RES_METAL,
    MUT_RESS_LIGHT = 0x01 << RES_LIGHT,
    MUT_RESS_CRYSTAL = 0x01 << RES_CRYSTAL,
    MUT_RESS_METEOR_CORE = 0x01 << RES_METEOR_CORE,

    __nb_mutex_ress = 6,
};
static_assert((int)__nb_mutex_ress == (int)__nb_ressources,
              "number of mutex and number of ressources are differents. If you added a ressource in the enum "
              "'tp_res', you should add it int the enum 'XXX' aswell");

int crafting_time[] = {
    [RES_DUST] = 0, [RES_ROCK] = 5, [RES_METAL] = 6, [RES_LIGHT] = 0, [RES_CRYSTAL] = 6, [RES_METEOR_CORE] = 9,
};

static pthread_mutex_t mutex[__nb_ressources];
int ressources[__nb_ressources];

static void acquire_mutex_ressources(uint8_t flag);
static void release_mutex_ressources(uint8_t flag);

struct aze {
    enum tp_res ress;
    int nb_ress;
};

int consume_ressource(uint8_t flag, struct aze ress_to_consume[], int n) {
    int error = 0;
    acquire_mutex_ressources(flag);

    for (int i = 0; i < n; i++) {
        if (ressources[ress_to_consume[i].ress] < ress_to_consume[i].nb_ress) {
            return -1;
        }
    }

    for (int i = 0; i < n; i++) {
        ressources[ress_to_consume[i].ress] -= ress_to_consume[i].nb_ress;
    }

    release_mutex_ressources(flag);

    return 0;
}

// struct aze a[] = {
//     {RES_DUST, 1},
//     {RES_ROCK, 3},
// };

int craft(struct aze in[], int in_n, struct aze result) {

    int flag = 0;

    for (int i = 0; i < in_n; i++) {
        flag = flag & (1 << in[i].ress);
    }

    if (consume_ressource(flag, in, in_n) != 0) {
        return -1;
    }

    sleep(crafting_time[result.ress]);

    acquire_mutex_ressources(0x01 << result.ress);
    ressources[result.ress] += result.nb_ress;
    release_mutex_ressources(0x01 << result.ress);

    return 0;
}

void craft_metal(tp_arg arg) {
    arg.worker->status = WT_CRAFTING_METAL;

    // TODO : Magic number
    int error = craft((struct aze[]){{.ress = RES_DUST, .nb_ress = 1}, {.ress = RES_ROCK, .nb_ress = 3}}, 2,
          (struct aze){.ress = RES_METAL, .nb_ress = 1});

    if (error) {
        arg.worker->status = WT_NOT_ENOUGHT_RESSOURCES;
        sleep(10); // TODO : Magic number
    }

    arg.worker->status = WT_IDLE;
}

void craft_rock(tp_arg arg) {

    arg.worker->status = WT_CRAFTING_ROCK;

    // TODO : Magic number
    int error = craft((struct aze[]){{.ress = RES_DUST, .nb_ress = 3}}, 1,
          (struct aze){.ress = RES_ROCK, .nb_ress = 1});

    if (error) {
        arg.worker->status = WT_NOT_ENOUGHT_RESSOURCES;
        sleep(10); // TODO : Magic number
    }

    arg.worker->status = WT_IDLE;

}

void craft_crystal(tp_arg arg) {
    arg.worker->status = WT_CRAFTING_CRYSTAL;

    // TODO : Magic number
    int error = craft((struct aze[]){{.ress=RES_DUST, .nb_ress=2} , {.ress = RES_LIGHT, .nb_ress = 1}}, 2,
          (struct aze){.ress = RES_CRYSTAL, .nb_ress = 1});

    if (error) {
        arg.worker->status = WT_NOT_ENOUGHT_RESSOURCES;
        sleep(10); // TODO : Magic number
    }

    arg.worker->status = WT_IDLE;
}

void craft_meteor_core(tp_arg arg) {
    arg.worker->status = WT_CRAFTING_METEOR_CORE;

    // TODO : Magic number
    int error = craft((struct aze[]){{.ress = RES_ROCK, .nb_ress = 1}, {.ress = RES_METAL, .nb_ress = 2}, {.ress = RES_CRYSTAL, .nb_ress = 2} }, 3,
          (struct aze){.ress = RES_METEOR_CORE, .nb_ress = 1});

    if (error) {
        arg.worker->status = WT_NOT_ENOUGHT_RESSOURCES;
        sleep(10); // TODO : Magic number
    }

    arg.worker->status = WT_IDLE;

}

void found_dusty_light(tp_arg arg) {

    arg.worker->status = WT_FOUND_DUSTYLIGHT;

    acquire_mutex_ressources(MUT_RESS_DUST | RES_LIGHT);

    ressources[RES_DUST] += 1;
    ressources[RES_LIGHT] += 1;

    release_mutex_ressources(MUT_RESS_DUST | RES_LIGHT);

    sleep(1);
    arg.worker->status = WT_IDLE;
}

void foundry_init() {
    ressources[RES_DUST] = 500;
    ressources[RES_LIGHT] = 50;
}

char *worker_task_to_string(enum tp_worker_task wt) {

    switch (wt) {
    case WT_IDLE:
        return "IDLE";
    case WT_NOT_ENOUGHT_RESSOURCES:
        return "NOT_ENOUGHT_RESSOURCES";
    case WT_CRAFTING_ROCK:
        return "CRAFTING_ROCK";
    case WT_CRAFTING_METAL:
        return "CRAFTING_METAL";
    case WT_CRAFTING_CRYSTAL:
        return "CRAFTING_CRYSTAL";
    case WT_CRAFTING_METEOR_CORE:
        return "CRAFTING_METEOR_CORE";
    case WT_FOUND_DUSTYLIGHT:
        return "FOUND_DUSTYLIGHT";
    default:
        return "UNREACHABLE HIT";
    };

    return "UNREACHABLE";
}

void submit_random_task(tp_arg arg) {
    int waiting_time = 0;

    struct tp_task task;
    fill_random_task(&task);
    tp_submit(task);
}

void fill_random_task(tp_task *task) {

    switch (rand() % 6) {
    case 0:
        task->pfun = craft_rock, task->arg = 0;
        break;
    case 1:
        task->pfun = craft_crystal, task->arg = 0;
        break;
    case 2:
        task->pfun = craft_meteor_core, task->arg = 0;
        break;
    case 3:
        task->pfun = craft_metal, task->arg = 0;
        break;
    case 4:
        task->pfun = found_dusty_light, task->arg = 0;
        break;
    case 5:
        task->pfun = submit_random_task, task->arg = 0;
        break;
    }
}

// ---

static void acquire_mutex_ressources(uint8_t flag) {

    for (int i = 0; i < __nb_mutex_ress; i++) {
        if (flag >> i & 0x01) {
            pthread_mutex_lock(&mutex[i]);
        }
    }
}

static void release_mutex_ressources(uint8_t flag) {

    for (int i = 0; i < __nb_mutex_ress; i++) {
        if ((flag >> i) & 0x01) {
            pthread_mutex_unlock(&mutex[i]);
        }
    }
}

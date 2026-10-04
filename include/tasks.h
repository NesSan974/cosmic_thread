#ifndef __foundry_h__
#define __foundry_h__

#include <pthread.h>
#include <stddef.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include <assert.h>

#include "tp.h"

enum tp_res {
    RES_DUST,
    RES_ROCK,
    RES_METAL,
    RES_LIGHT,
    RES_CRYSTAL,
    RES_METEOR_CORE,
    __nb_ressources,
    // ---
    // RES_SOLAR_FLARE,
    // RES_GAS,
};


enum tp_worker_task {
    WT_IDLE,
    WT_NOT_ENOUGHT_RESSOURCES,

    WT_CRAFTING_ROCK,
    WT_CRAFTING_METAL,
    WT_CRAFTING_CRYSTAL,
    WT_CRAFTING_METEOR_CORE,

    WT_FOUND_DUSTYLIGHT,
};

extern int ressources[__nb_ressources];

void foundry_init();

void craft_metal(tp_arg arg);
void craft_rock(tp_arg arg);
void craft_crystal(tp_arg arg);
void craft_meteor_core(tp_arg arg);

void found_dusty_light(tp_arg arg);

char* worker_task_to_string(enum tp_worker_task wt);

void submit_random_task(tp_arg arg);
void fill_random_task(tp_task *task);

#endif // __foundry_h__

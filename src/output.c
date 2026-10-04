#include "output.h"

static void display_ressources();
static void display_worker();
static void display_commande_craft();
static void display_task_queue();

static char *command_craft[][3] = {
    {"'a': ", "3 × dust", "1 × rock"},
    {"'z': ", "3 × rock + 1 × dust", "1 × metal"},
    {"'e': ", "2 × dust + 1 × light", "1 × crystal"},
    {"'r': ", "2 × metal + 2 × crystal + 1 × rock", "1 × meteor_core"},
};

void clear_term() {
    printf("\033[2J");
    fflush(stdout);
}

void output_display() {
    clear_term();
    display_ressources();
    display_worker();
    display_commande_craft();
    display_task_queue();
}

// ---

static void display_ressources() {
    printf("\e[1;1H" "%30s%-8s|%8s|%8s|%8s|%8s|%12s", "", "DUST", "ROCK", "METAL", "LIGHT", "CRYSTAL", "METOR CORE");
    printf("\e[2;1H" "%30s%-8d|%8d|%8d|%8d|%8d|%12d", "", ressources[RES_DUST], ressources[RES_ROCK], ressources[RES_METAL], ressources[RES_LIGHT],
           ressources[RES_CRYSTAL], ressources[RES_METEOR_CORE]);

    fflush(stdout);
}

static void display_worker() {
    int i = 0;
    for (size_t i = 0; i < NB_THREAD; i++) {
        printf("\e[%ld;%dH", 4 + i, 0);
        printf("w%ld: %-25s", i, worker_task_to_string(thread_pool[i].status));
    }

    fflush(stdout);
}

static void display_commande_craft() {
    size_t i = 0;

    for (i = 0; i < sizeof(command_craft) / sizeof(command_craft[0]); i++) {
        printf("\e[%ld;%dH", 4 + i, 40);
        printf("%-5s%-40s -> %-20s", command_craft[i][0], command_craft[i][1], command_craft[i][2]);
    }

    printf("\e[%ld;%dH", 4 + i, 35);
    printf("'space' : to have random task, can find light and dust\n");
    fflush(stdout);
}

static void display_task_queue() {
    int base = 3 + NB_THREAD + 3;

    printf("\e[%d;%dH", base++, 0);
    printf("Next Task (tot : %ld):", task_queue.count);

    for (size_t i = 0; i < 5; i++) {
        printf("\e[%ld;%dH", base + i, 0);
        printf("- %p", task_queue.items[task_queue.head + i].pfun);
    }

    fflush(stdout);
}

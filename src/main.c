#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <termios.h>

#include "tasks.h"
#include "output.h"
#include "tp.h"

#define NB_MAX_TASK 2048

void setup();
void handle_input(char *c);

struct termios old;
struct termios raw;

int main(int argc, char **arv) {

    setup();

    char c;
    while (1) {
        handle_input(&c);

        if (c == 'q') {
            break;
        }

        output_display();

        printf("\e[18;30H" "'q' for quit\n");

        usleep(200000);
    }

    return 0;
}


// to insta get keychar pressed without pressing enter
void termios_setup() {

    // Save current config
    tcgetattr(STDIN_FILENO, &old);
    raw = old;
    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void handle_input(char *c) {
    if (read(STDIN_FILENO, c, 1) == 1) {
        if (*c == 'a') {
            tp_submit((tp_task){.pfun = craft_rock});
        } else if (*c == 'z') {
            tp_submit((tp_task){.pfun = craft_metal});
        } else if (*c == 'e') {
            tp_submit((tp_task){.pfun = craft_crystal});
        } else if (*c == 'r') {
            tp_submit((tp_task){.pfun = craft_meteor_core});
        } else if (*c == ' ') {
            submit_random_task((tp_arg){0});
        }
    }
}

void restore_terminal(void) {
    printf("\e[?25h"); // show cursor
    printf("\e[?25r"); // restore old state of the cursor
    printf("\e[0m"); // rest style and color

    fflush(stdout);

    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}

void handle_signal(int sig) {
    restore_terminal();
    _exit(128 + sig);
}

void setup() {
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);   // stdin in non block

    signal(SIGINT, handle_signal);              // To reset console properly even if ctrl-C
    atexit(restore_terminal);
    srand(time(NULL));

    printf("\e[?25s");                          // save current state of the cursor
    printf("\e[?25l");                          // don't show cursor

    tp_init(NB_MAX_TASK);
    foundry_init();

    termios_setup();
}

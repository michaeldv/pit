#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

PTable projects;
PTable tasks;
PTable notes;
PTable actions;

static int usage() {
    printf("usage...\n");
    return 1;
}

/*
** Suicide.
*/
void die(char *message, ...)
{
    char str[4096];
    va_list params;

    va_start(params, message);
      vsnprintf(str, sizeof(str), message, params);
      fprintf(stderr, "pit: %s\n", str);
    va_end(params);

    exit(0);
}
/*
** Forceful death.
*/
void perish(char *prefix)
{
    fprintf(stderr, "pit (fatal): ");
    if (prefix) {
        fprintf(stderr, "%s - ", prefix);
    }
    perror(NULL);
    exit(0);
}

int main(int argc, char *argv[]) {
    char *commands[] = { "project", "task", "note", "log", "init" };

    /***
    int i;
    printf("argc: %d\n", argc);
    for(i = 0;  i < argc;  i++) {
        printf("argv[%d]: [%s]\n", i, argv[i]);
    }
    ***/

    if (argc > 1) {
        if (strstr(commands[0], argv[1]) == commands[0]) {
            return pit_project(&argv[1]);
        } else if (strstr(commands[1], argv[1]) == commands[1]) {
            return pit_task(&argv[1]);
        } else if (strstr(commands[2], argv[1]) == commands[2]) {
            return 1; /* pit_note(&argv[1]); */
        } else if (strstr(commands[3], argv[1]) == commands[3]) {
            return pit_log(&argv[1]);
        } else if (strstr(commands[4], argv[1]) == commands[4]) {
            return pit_init(&argv[1]);
        }
    }
    return usage();
}
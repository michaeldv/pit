#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "pit.h"
#include "project.h"
#include "task.h"

PTable projects;
PTable tasks;
PTable notes;
PTable activities;
PTable users;

static int usage() {
    printf("usage...\n");
    return 1;
}

void die(char *msg)
{
    fprintf(stderr, "pit (fatal): %s\n", msg);
    exit(0);
}

void die_with_errno(char *prefix)
{
    fprintf(stderr, "pit (fatal): %s: ", prefix);
    perror(NULL);
    exit(0);
}

int main(int argc, char *argv[]) {
    char *commands[] = { "project", "task", "note", "log" };

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
        }
    }
    return usage();
}
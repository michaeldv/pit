#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static int usage() {
    printf("usage...\n");
    return 1;
}

int main(int argc, char *argv[]) {
    int i;
    char *commands[] = { "project", "task", "note", "log" };

    printf("argc: %d\n", argc);
    for(i = 0;  i < argc;  i++) {
        printf("argv[%d]: [%s]\n", i, argv[i]);
    }

    if (argc > 1) {
        if (strstr(commands[0], argv[1]) == commands[0]) {
            return pit_project(&argv[1]);
        } else if (strstr(commands[1], argv[1]) == commands[1]) {
            return pit_task(&argv[1]);
        }
    }
    return usage();
}
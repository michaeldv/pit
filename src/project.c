#include "pit.h"
#include "project.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int pit_project(char *argv[]) {
    char **arg = &argv[1];
    char *error = NULL;

    if (!*arg) {
        printf("list projects\n");
    } else if (!strcmp(*arg, "-c")) {
        if (!*++arg) {
            error = "missing project name";
        } else {
            create_project(*arg, *(arg + 1));
        }
    } else if (!strcmp(*arg, "-d") || !strcmp(*arg, "-s")) {
        if (!*++arg) {
            error = "missing project number";
        } else {
            unsigned long number = atoi(*arg);
            if (!number) {
                error = "invalid project number";
            } else {
                if (!strcmp(*--arg, "-d")) {
                    delete_project(number);
                } else {
                    show_project(number);
                }
            }
        }
    }

    if (error) {
        printf("%s\n", error);
    }

    return error == NULL;
}

static int create_project(char *name, char *status) {
    printf("creating project [%s], status [%s]\n", name, status);
    return 1;
}

static int show_project(unsigned long number) {
    printf("showing project %lu\n", number);
    return 1;
}

static int delete_project(unsigned long number) {
    printf("deleting project %lu\n", number);
    return 1;
}


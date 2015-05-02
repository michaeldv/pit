/*
** Copyright (c) 2010 Michael Dvorkin
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the Simplified BSD License (also
** known as the "2-Clause License" or "FreeBSD License".)
**
** This program is distributed in the hope that it will be useful,
** but without any warranty; without even the implied warranty of
** merchantability or fitness for a particular purpose.
*/
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

PHeader header   = NULL;
PTable  projects = NULL;
PTable  tasks    = NULL;
PTable  notes    = NULL;
PTable  actions  = NULL;

void free_externals() {
    if (header)   free(header);
    if (actions)  pit_table_free(actions);
    if (notes)    pit_table_free(notes);
    if (projects) pit_table_free(projects);
    if (tasks)    pit_table_free(tasks);
}

/*
** Suicide.
*/
void die(char *message, ...)
{
    char str[4096];
    va_list params;

    if (message) {
        va_start(params, message);
          vsnprintf(str, sizeof(str), message, params);
          fprintf(stderr, "pit: %s\n", str);
        va_end(params);
    }

    free_externals();
    exit(1);
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

    free_externals();
    exit(1);
}

void pit_version(char *argv[])
{
    printf("pit version %s\n", PIT_VERSION);
}

int main(int argc, char *argv[]) {
    register int i, candidate = -1;
    char *command[] = { "project", "task", "note", "log", "init", "info", "help", "version" };
    void (*handler[])(char *argv[]) = { pit_project, pit_task, pit_note, pit_log, pit_init, pit_info, pit_help, pit_version };

    if (argc == 1) { argv[1] = "help"; argv[2] = NULL; }

    for(i = 0;  i < ARRAY_SIZE(command);  i++) {
        if (strstr(command[i], argv[1]) == command[i]) {
            if (candidate < 0) {
                candidate = i;
            } else {
                die("ambiguous command (%s)", argv[1]);
            }
        }
    }

    if (candidate < 0)
        die("invalid command (%s), run 'pit help' for help", argv[1]);

    handler[candidate](&argv[1]);
    free_externals();
    return 0;
}

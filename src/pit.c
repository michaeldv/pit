#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

PTable projects;
PTable tasks;
PTable notes;
PTable actions;

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

void pit_status(char *argv[])
{
    puts("pit: status is not implemented yet");
}

int main(int argc, char *argv[]) {
    register int i;
    char *commands[] = { "project", "task", "note", "log", "init", "status", "help" };
    void (*handlers[])(char *argv[]) = { pit_project, pit_task, pit_note, pit_log, pit_init, pit_status, pit_help };

    if (argc == 1) argv[1] = "help";
    for(i = 0;  i < ARRAY_SIZE(commands);  i++) {
        if (strstr(commands[i], argv[1]) == commands[i]) {
            handlers[i](&argv[1]);
            return 1;
        }
    }
    printf("invalid command: %s", argv[1]);
    return 0;
}

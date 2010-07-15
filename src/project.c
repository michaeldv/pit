#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "pit.h"
#include "db.h"
#include "project.h"

static int already_exist(char *name)
{
    ulong i;
    PProject pp;

    pit_db_load();
    for(i = 0, pp = (PProject)projects->slots;  i < projects->number_of_records;  i++, pp++) {
        if (!strcmp(pp->name, name)) {
            return 1;
        }
    }
    return 0;
}

static void list_projects()
{
    ulong i;
    PProject pp;

    pit_db_load();
    for(i = 0, pp = (PProject)projects->slots;  i < projects->number_of_records;  i++, pp++) {
        printf("%lu: [%s] %s\n", pp->id, pp->status, pp->name);
    }
}

static void create_project(char *name, char *status)
{
    pit_db_load();
    printf("creating project [%s], status [%s]\n", name, status);

    if (!already_exist(name)) {
        Project p;

        memset(&p, 0, sizeof(p));

        strncpy(p.name, name, sizeof(p.name) - 1);
        strncpy(p.status, status, sizeof(p.status) - 1);
        p.current = 1;
        p.number_of_open_tasks = 0;
        p.number_of_closed_tasks = 0;
        p.closed_by = 0;
        p.created_by = p.updated_by = 1; // TODO
        p.closed_at = 0;
        p.created_at = p.updated_at = time(NULL);
        pit_table_insert(projects, (uchar *)&p);
        pit_db_save();
    }
}

static int show_project(unsigned long number)
{
    printf("showing project %lu\n", number);
    return 1;
}

static int delete_project(unsigned long number)
{
    printf("deleting project %lu\n", number);
    return 1;
}

int pit_project(char *argv[])
{
    char **arg = &argv[1];
    char *error = NULL;

    if (!*arg) {
        list_projects();
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

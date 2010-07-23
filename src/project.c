#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

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
        printf("%c %lu: %s (%s, %lu open task%s)\n", (pp->id == projects->current ? '*' : ' '),
        pp->id, pp->name, pp->status,
        pp->number_of_open_tasks, (pp->number_of_open_tasks != 1 ? "s" : ""));
    }
}

static void create_project(char *name, char *status)
{
    pit_db_load();

    if (already_exist(name)) {
        die("project with the same name already exists");
    } else {
        Project p, *pp;

        memset(&p, 0, sizeof(p));

        if (!status) {
            status = "active";
        }
        printf("creating project [%s], status [%s]\n", name, status);

        strncpy(p.name, name, sizeof(p.name) - 1);
        strncpy(p.status, status, sizeof(p.status) - 1);
        p.number_of_open_tasks = 0;
        p.number_of_closed_tasks = 0;
        p.closed_by = 0;
        p.created_by = p.updated_by = 1; // TODO
        p.closed_at = 0;
        p.created_at = p.updated_at = time(NULL);
        pp = (PProject)pit_table_insert(projects, (uchar *)&p);
        pit_table_mark(projects, pp->id);
        pit_db_save();
    }
}

static int show_project(ulong number)
{
    PProject pp;

    pit_db_load();
    pp = (PProject)pit_table_find(projects, number);
    if (pp) {
        printf("%lu: %s (%s, %lu open task%s, %lu closed task%s)\n",
            pp->id, pp->name, pp->status,
            pp->number_of_open_tasks, (pp->number_of_open_tasks != 1 ? "s" : ""),
            pp->number_of_closed_tasks, (pp->number_of_closed_tasks != 1 ? "s" : ""));
        if (pp->number_of_open_tasks > 0) {
            ulong i;
            PTask pt = (PTask)tasks->slots;

            puts("Open tasks:");
            for(i = 0;  i < tasks->number_of_records;  i++, pt++) {
                if (pt->closed_at) {
                    continue;
                }
                printf("  %c %lu: %s (%lu notes)\n", (pt->id == tasks->current ? '*' : ' '), pt->id, pt->name, pt->number_of_notes);
            }
        }
        pit_table_mark(projects, pp->id);
        pit_db_save();
    } else {
        die("could not find the project");
    }
    return 1;
}

static int delete_project(unsigned long number)
{
    PProject pp;

    printf("deleting project %lu\n", number);
    pit_db_load();
    pp = (PProject)pit_table_delete(projects, number);
    if (pp) {
        pit_table_mark(projects, 0);
        pit_db_save();
    } else {
        die("could not delete the project");
    }
    return 1;
}

int pit_project(char *argv[])
{
    char **arg = &argv[1];
    unsigned long number;

    if (!*arg) {
        list_projects();
    } else if (!strcmp(*arg, "-c")) {
        if (!*++arg) {
            die("missing project name");
        } else {
            create_project(*arg, *(arg + 1));
        }
    } else if (!strcmp(*arg, "-d")) {
        if (!*++arg) {
            die("missing project number");
        } else {
            number = atoi(*arg);
            if (!number) {
                die("invalid project number");
            } else {
                delete_project(number);
            }
        }
 /* } else if (!strcmp(*arg, "-e")) { TODO: Edit */
    } else {
        number = atoi(*arg);
        if (!number) {
            die("invalid project parameters");
        } else {
            show_project(number);
        }
    }

    return 1;
}

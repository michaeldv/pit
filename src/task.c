#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "pit.h"
#include "db.h"
#include "task.h"

static void list_tasks()
{
    pit_db_load();

    ulong i = 0;
    PTask pt = (PTask)tasks->slots;
    PProject pp = (PProject)pit_table_current(projects);

    for(; i < tasks->number_of_records; i++, pt++) {
        if (pp && pt->project_id != pp->id) {
            continue;
        }
        printf("%c %lu: [%s] %s (%lu notes)\n", (pt->id == tasks->current ? '*' : ' '), pt->id, pt->status, pt->name, pt->number_of_notes);
    }
}

static void create_task(char *name, char *status)
{
    pit_db_load();

    PProject pp = (PProject)pit_table_current(projects);

    if (!pp) {
        die("no project selected");
    } else {
        Task t, *pt;

        memset(&t, 0, sizeof(t));

        if (!status) {
            status = "open";
        }
        printf("creating task [%s], status [%s]\n", name, status);

        strncpy(t.name, name, sizeof(t.name) - 1);
        strncpy(t.status, status, sizeof(t.status) - 1);
        t.project_id = pp->id;
        t.priority = 1;
        t.deadline = time(NULL);
        t.number_of_notes = 0;
        t.closed_by = 0;
        t.created_by = t.updated_by = 1; // TODO
        t.closed_at = 0;
        t.created_at = t.updated_at = time(NULL);

        pt = (PTask)pit_table_insert(tasks, (uchar *)&t);
        pit_table_mark(tasks, pt->id);
        pp->number_of_open_tasks++;
        pit_db_save();
    }
}

static int show_task(ulong number)
{
    return 1;
}

static int delete_task(ulong number)
{
    return 1;
}

int pit_task(char *argv[])
{
    char **arg = &argv[1];
    unsigned long number;

    if (!*arg) {
        list_tasks();
    } else if (!strcmp(*arg, "-c")) {
        if (!*++arg) {
            die("missing task name");
        } else {
            create_task(*arg, *(arg + 1));
        }
    } else if (!strcmp(*arg, "-d")) {
        if (!*++arg) {
            die("missing task number");
        } else {
            number = atoi(*arg);
            if (!number) {
                die("invalid task number");
            } else {
                delete_task(number);
            }
        }
 /* } else if (!strcmp(*arg, "-e")) { TODO: Edit */
    } else {
        number = atoi(*arg);
        if (!number) {
            die("invalid task parameters");
        } else {
            show_task(number);
        }
    }

    return 1;
}

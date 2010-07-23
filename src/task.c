#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

/*
** CREATING TASKS:
**   pit task -c name [-s status] [-d deadline] [-p priority]
**
** UPDATING TASKS:
**   pit task -u [number] [-n name] [-s status] [-d deadline] [-p priority]
**
** DELETING TASKS:
**   pit task -d [number]
**
** SHOWING TASK:
**   pit task [-s number]
**
** LISTING TASKS:
**   pit task -s [all|open|closed] [-s status] [-d deadline] [-p priority]
*/
static void list_tasks(char *mode, char *status, char *deadline, ulong priority)
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

static void create_task(char *name, char *status, char *priority, time_t deadline)
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
        if (!priority) {
            priority = "normal";
        }

        printf("creating task: %s, status: %s, priority: %s, deadline: %s", name, status, priority, ctime(&deadline));

        strncpy(t.name, name, sizeof(t.name) - 1);
        strncpy(t.status, status, sizeof(t.status) - 1);
        strncpy(t.priority, priority, sizeof(t.priority) - 1);
        t.project_id = pp->id;
        t.deadline = deadline;
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

static void show_task(ulong number)
{
    printf("show_task(%lu)\n", number);
}

static void delete_task(ulong number)
{
    printf("delete_task(%lu)\n", number);
}

static void update_task(ulong number, char *name, char *status, char *priority, time_t deadline)
{
    printf("update task: #%lu, name: %s, status: %s, priority: %s, deadline: %s", number, name, status, priority, ctime(&deadline));
}

int pit_task(char *argv[])
{
    char **arg = &argv[1];
    char *name = NULL, *status = NULL, *priority = NULL;
    time_t deadline = (time_t)0;
    ulong number = 0;

    if (!*arg) {
        list_tasks(NULL, NULL, NULL, 0); /* ...with default paramaters. */
    } else {
        switch(pit_arg_option(arg)) {
        case 'c': /* pit task -c name [-s status] [-p priority] [-d deadline] */
            name = pit_arg_string(++arg, "task name");
            while(*++arg) {
                switch(pit_arg_option(arg)) {
                case 's':
                    status = pit_arg_string(++arg, "task status");
                    break;
                case 'p':
                    priority = pit_arg_string(++arg, "task priority");
                    break;
                case 'd':
                    deadline = pit_arg_time(++arg, "task deadline");
                    break;
                default:
                    die("invalid task option: %s", *arg);
                }
            }
            create_task(name, status, priority, deadline);
            break;
        case 'u': /* pit task -u [number] [-n name] [-s status] [-d deadline] [-p priority] */
            number = pit_arg_number(++arg, NULL);
            if (!number) --arg;
            while(*++arg) {
                switch(pit_arg_option(arg)) {
                case 'n':
                    name = pit_arg_string(++arg, "task name");
                    break;
                case 's':
                    status = pit_arg_string(++arg, "task status");
                    break;
                case 'p':
                    priority = pit_arg_string(++arg, "task priority");
                    break;
                case 'd':
                    deadline = pit_arg_time(++arg, "task deadline");
                    break;
                default:
                    die("invalid task option: %s", *arg);
                }
            }
            if (!name && !status && !priority && !deadline) {
                die("nothing to update");
            } else {
                update_task(number, name, status, priority, deadline);
            }
            break;
        case 'd': /* pit task -d [number] */
            number = pit_arg_number(++arg, NULL);
            delete_task(number);
            break;
        case 's': /* pit task -s [all|open|closed] [-s status] [-d deadline] [-p priority] */
            break;
        default: /* pit task [number] */
            number = pit_arg_number(++arg, NULL);
            show_task(number);
        }
    }
    return 1;
}

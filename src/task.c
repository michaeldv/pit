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
** VIEWING TASK:
**   pit task [[-v] number]
**
** LISTING TASKS:
**   pit task -v [-s status] [-d deadline] [-p priority]
*/

static void list_tasks(char *name, char *status, char *priority, time_t deadline)
{
    pit_db_load();

    PTask pt = (PTask)tasks->slots;
    PProject pp = (PProject)pit_table_current(projects);

    for_each_task(pt) {
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
    printf("update_task: #%lu, name: %s, status: %s, priority: %s, deadline: %s", number, name, status, priority, ctime(&deadline));
}

static void get_status_priority_and_deadline(char **arg, char *status, char *priority, time_t *deadline)
{
    while(*++arg) {
        switch(pit_arg_option(arg)) {
        case 's':
            status = pit_arg_string(++arg, "task status");
            break;
        case 'p':
            priority = pit_arg_string(++arg, "task priority");
            break;
        case 'd':
            *deadline = pit_arg_time(++arg, "task deadline");
            break;
        default:
            die("invalid task option: %s", *arg);
        }
    }
}

static void get_name_status_priority_and_deadline(char **arg, char *name, char *status, char *priority, time_t *deadline)
{
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
            *deadline = pit_arg_time(++arg, "task deadline");
            break;
        default:
            die("invalid task option: %s", *arg);
        }
    }
}

int pit_task(char *argv[])
{
    char **arg = &argv[1];
    char *name = NULL, *status = NULL, *priority = NULL;
    time_t deadline = (time_t)0;
    ulong number = 0L;

    if (!*arg) {
        list_tasks(NULL, NULL, NULL, 0); /* List all tasks (with default paramaters). */
    } else { /* pit task [number] */
        number = pit_arg_number(arg, NULL);
        if (number) {
            show_task(number);
        } else {
            switch(pit_arg_option(arg)) {
            case 'c': /* pit task -c name [-s status] [-p priority] [-d deadline] */
                name = pit_arg_string(++arg, "task name");
                get_status_priority_and_deadline(arg, status, priority, &deadline);
                create_task(name, status, priority, deadline);
                break;
            case 'u': /* pit task -u [number] [-n name] [-s status] [-d deadline] [-p priority] */
                number = pit_arg_number(++arg, NULL);
                if (!number) --arg;
                get_name_status_priority_and_deadline(arg, name, status, priority, &deadline);
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
            case 'v': /* pit task -v [number | [-n name] [-s status] [-d deadline] [-p priority]] */
                number = pit_arg_number(++arg, NULL);
                if (number) {
                    show_task(number);
                } else {
                    get_name_status_priority_and_deadline(--arg, name, status, priority, &deadline);
                    if (!name && !status && !priority && !deadline) {
                        show_task(0);
                    } else {
                        list_tasks(name, status, priority, deadline);
                    }
                }
                break;
            default:
                die("invalid task option: %s", *arg);
            }
        }
    }
    return 1;
}

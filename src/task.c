#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static void task_list(char *name, char *status, char *priority, time_t deadline);
static void task_create(char *name, char *status, char *priority, time_t deadline);
static void task_show(ulong number);
static void task_delete(ulong number);
static void task_update(ulong number, char *name, char *status, char *priority, time_t deadline);
static void task_parse_options(char **arg, char **name, char **status, char **priority, time_t *deadline);

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
**   pit task [[-q] number]
**
** LISTING TASKS:
**   pit task -q [-s status] [-d deadline] [-p priority]
*/

static void task_list(char *name, char *status, char *priority, time_t deadline)
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

static void task_create(char *name, char *status, char *priority, time_t deadline)
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

static void task_show(ulong number)
{
    printf("task_show(%lu)\n", number);
}

static void task_delete(ulong number)
{
    printf("task_delete(%lu)\n", number);
}

static void task_update(ulong number, char *name, char *status, char *priority, time_t deadline)
{
    printf("task_update: #%lu, name: %s, status: %s, priority: %s, deadline: %s", number, name, status, priority, ctime(&deadline));
}

static void task_parse_options(char **arg, char **name, char **status, char **priority, time_t *deadline)
{
    while(*++arg) {
        switch(pit_arg_option(arg)) {
        case 's':
            *status = pit_arg_string(++arg, "task status");
            break;
        case 'p':
            *priority = pit_arg_string(++arg, "task priority");
            break;
        case 'd':
            *deadline = pit_arg_time(++arg, "task deadline");
            break;
        case 'n':
            if (name) {
                *name = pit_arg_string(++arg, "task name");
                break;
            } /* else fall though */
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
        task_list(NULL, NULL, NULL, 0); /* List all tasks (with default paramaters). */
    } else { /* pit task [number] */
        number = pit_arg_number(arg, NULL);
        if (number) {
            task_show(number);
        } else {
            switch(pit_arg_option(arg)) {
            case 'c': /* pit task -c name [-s status] [-p priority] [-d deadline] */
                name = pit_arg_string(++arg, "task name");
                task_parse_options(arg, NULL, &status, &priority, &deadline);
                task_create(name, status, priority, deadline);
                break;
            case 'u': /* pit task -u [number] [-n name] [-s status] [-d deadline] [-p priority] */
                number = pit_arg_number(++arg, NULL);
                if (!number) --arg;
                task_parse_options(arg, &name, &status, &priority, &deadline);
                if (!name && !status && !priority && !deadline) {
                    die("nothing to update");
                } else {
                    task_update(number, name, status, priority, deadline);
                }
                break;
            case 'd': /* pit task -d [number] */
                number = pit_arg_number(++arg, NULL);
                task_delete(number);
                break;
            case 'q': /* pit task -q [number | [-n name] [-s status] [-d deadline] [-p priority]] */
                number = pit_arg_number(++arg, NULL);
                if (number) {
                    task_show(number);
                } else {
                    task_parse_options(--arg, &name, &status, &priority, &deadline);
                    if (!name && !status && !priority && !deadline) {
                        task_show(0);
                    } else {
                        task_list(name, status, priority, deadline);
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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "pit.h"

static void task_list(char *name, char *status, char *priority, time_t date, time_t time);
static void task_create(char *name, char *status, char *priority, time_t date, time_t time);
static void task_show(ulong number);
static void task_delete(ulong number);
static void task_update(ulong number, char *name, char *status, char *priority, time_t date, time_t time);
static void task_log(char *message, ulong id, ...);
static void task_parse_options(char **arg, char **name, char **status, char **priority, time_t *date, time_t *time);

/*
** CREATING TASKS:
**   pit task -c name [-s status] [-p priority] [-d date] [-t time]
**
** UPDATING TASKS:
**   pit task -u [number] [-n name] [-s status] [-p priority] [-d date] [-t time]
**
** DELETING TASKS:
**   pit task -d [number]
**
** VIEWING TASK:
**   pit task [[-q] number]
**
** LISTING TASKS:
**   pit task -q [-s status] [-p priority] [-d date] [-t time]
*/

static void task_list(char *name, char *status, char *priority, time_t date, time_t time)
{
    PProject pp;
    PTask    pt;
    PPager   ppager;

    pit_db_load();
    if (tasks->number_of_records > 0) {
        pp = (PProject)pit_table_current(projects);
        ppager = pit_pager_initialize(PAGER_TASK, tasks->number_of_records);
        for_each_task(pt) {
            if (pp && pt->project_id != pp->id) {
                continue;
            }
            pit_pager_print(ppager, (uchar *)pt);
        }
        pit_pager_flush(ppager);
    }
}

static void task_create(char *name, char *status, char *priority, time_t date, time_t time)
{
    pit_db_load();

    PProject pp = (PProject)pit_table_current(projects);

    if (!pp) {
        die("no project selected");
    } else {
        Task t, *pt;

        memset(&t, 0, sizeof(t));

        if (!status) status = "open";
        if (!priority) priority = "normal";

        printf("creating task: %s, status: %s, priority: %s, date: %s", name, status, priority, ctime(&date));

        strncpy(t.name, name, sizeof(t.name) - 1);
        strncpy(t.status, status, sizeof(t.status) - 1);
        strncpy(t.priority, priority, sizeof(t.priority) - 1);
        strncpy(t.username, current_user(), sizeof(t.username) - 1);
        t.project_id = pp->id;
        t.date = date;
        t.time = time;

        pt = (PTask)pit_table_insert(tasks, (uchar *)&t);
        pit_table_mark(tasks, pt->id);
        pp->number_of_tasks++;
        // pit_action(pit->id, "task", "created task #%d: %s (status: %s, priority: %s, date: %s)", pt->id, name, status, priority, (date ? ctime(&date) : "n/a"));
        task_log("created task #%d: %s (status: %s, priority: %s, date: %s)", pt->id, name, status, priority, (date ? ctime(&date) : "n/a"));
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

static void task_update(ulong number, char *name, char *status, char *priority, time_t date, time_t time)
{
    printf("task_update: #%lu, name: %s, status: %s, priority: %s, date: %s", number, name, status, priority, ctime(&date));
}

static void task_log(char *message, ulong id, ...)
{
    char str[128];
    va_list params;

    va_start(params, id);
      vsnprintf(str, sizeof(str), (char *)&id, params);
      pit_action(id, "task", str);
    va_end(params);
}

static void task_parse_options(char **arg, char **name, char **status, char **priority, time_t *date, time_t *time)
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
            *date = pit_arg_date(++arg, "task date");
            break;
        case 't':
            *time = pit_arg_time(++arg, "task time");
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
    time_t date = (time_t)0;
    time_t time = (time_t)0;
    ulong number = 0L;

    if (!*arg) {
        task_list(NULL, NULL, NULL, 0, 0); /* List all tasks (with default paramaters). */
    } else { /* pit task [number] */
        number = pit_arg_number(arg, NULL);
        if (number) {
            task_show(number);
        } else {
            switch(pit_arg_option(arg)) {
            case 'c': /* pit task -c name [-s status] [-p priority] [-d date] [-t time] */
                name = pit_arg_string(++arg, "task name");
                task_parse_options(arg, NULL, &status, &priority, &date, &time);
                task_create(name, status, priority, date, time);
                break;
            case 'u': /* pit task -u [number] [-n name] [-s status] [-p priority] [-d date] [-t time]*/
                number = pit_arg_number(++arg, NULL);
                if (!number) --arg;
                task_parse_options(arg, &name, &status, &priority, &date, &time);
                if (!name && !status && !priority && !date && !time) {
                    die("nothing to update");
                } else {
                    task_update(number, name, status, priority, date, time);
                }
                break;
            case 'd': /* pit task -d [number] */
                number = pit_arg_number(++arg, NULL);
                task_delete(number);
                break;
            case 'q': /* pit task -q [number | [-n name] [-s status] [-p priority] [-d date] [-t time]] */
                number = pit_arg_number(++arg, NULL);
                if (number) {
                    task_show(number);
                } else {
                    task_parse_options(--arg, &name, &status, &priority, &date, &time);
                    if (!name && !status && !priority && !date && !time) {
                        task_show(0);
                    } else {
                        task_list(name, status, priority, date, time);
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

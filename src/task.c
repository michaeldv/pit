#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static void task_list(char *name, char *status, char *priority, time_t date, time_t time);
static void task_show(ulong number);
static void task_create(char *name, char *status, char *priority, time_t date, time_t time);
static void task_update(ulong number, char *name, char *status, char *priority, time_t date, time_t time);
static void task_delete(ulong number);
static void task_log_create(PTask pt, char *name, char *status, char *priority, time_t date, time_t time);
static void task_log_update(PTask pt, char *name, char *status, char *priority, time_t date, time_t time);
static void task_log_delete(PTask pt);
static void task_parse_options(char **arg, char **name, char **status, char **priority, time_t *date, time_t *time);

/*
** CREATING TASKS:
**   pit task -c name [-s status] [-p priority] [-d date] [-t time]
**
** EDITING TASKS:
**   pit task -e [number] [-n name] [-s status] [-p priority] [-d date] [-t time]
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

static void task_show(ulong number)
{
    printf("task_show(%lu)\n", number);
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
        task_log_create(pt, name, status, priority, date, time);
        pit_db_save();
    }
}

static void task_update(ulong number, char *name, char *status, char *priority, time_t date, time_t time)
{
    PTask pt;

    pit_db_load();
    if (number) {
        pt = (PTask)pit_table_find(tasks, number);
        if (!pt) die("could not find task %lu", number);
    } else {
        pt = (PTask)pit_table_current(tasks);
        if (!pt) die("could not find current task");
    }
    if (name) strncpy(pt->name, name, sizeof(pt->name) - 1);
    if (status) strncpy(pt->status, status, sizeof(pt->status) - 1);
    if (priority) strncpy(pt->priority, priority, sizeof(pt->priority) - 1);
    if (date) pt->date = date;
    if (time) pt->time = time;
    pit_table_mark(tasks, pt->id);

    task_log_update(pt, name, status, priority, date, time);
    pit_db_save();
}

static void task_delete(ulong number)
{
    PTask pt;
    PNote pn;

    pit_db_load();
    pt = (PTask)pit_table_delete(tasks, number);
    if (pt) {
        pit_table_mark(tasks, 0);
        if (pt->number_of_notes > 0) {
            for_each_note(pn) {
                if (pn->task_id == pt->id) {
                    pit_table_delete(notes, pn->id);
                }
            }
        }
        task_log_delete(pt);
        pit_db_save();
    } else {
        die("could not delete task %lu", number);
    }
}

static void task_log_create(PTask pt, char *name, char *status, char *priority, time_t date, time_t time)
{
    char str[256];

    sprintf(str, "created task %lu: %s (status: %s, priority: %s", pt->id, name, status, priority);
    if (date) sprintf(str + strlen(str), ", date: %s", format_date(date));
    if (time) sprintf(str + strlen(str), ", time: %s", format_date(time));
    strcat(str, ")");
    puts(str);
    pit_action(pt->id, "task", str);
}

static void task_log_update(PTask pt, char *name, char *status, char *priority, time_t date, time_t time)
{
    char str[256];
    bool empty = TRUE;

    sprintf(str, "updated task %lu:", pt->id);
    if (name) {
        sprintf(str + strlen(str), " (name: %s", name);
        empty = FALSE;
    } else {
        sprintf(str + strlen(str), " %s (", pt->name);
    }
    if (status) {
        sprintf(str + strlen(str), "%sstatus: %s", (empty ? "" : ", "), status);
        empty = FALSE;
    }
    if (priority) {
        sprintf(str + strlen(str), "%spriority: %s", (empty ? "" : ", "), priority);
        empty = FALSE;
    }
    if (date) {
        sprintf(str + strlen(str), "%sdate: %s", (empty ? "" : ", "), format_date(date));
        empty = FALSE;
    }
    if (time) sprintf(str + strlen(str), "%stime: %s", (empty ? "" : ", "), format_date(time));
    strcat(str, ")");
    puts(str);
    pit_action(pt->id, "task", str);
}

static void task_log_delete(PTask pt)
{
    char str[256];
    
    sprintf(str, "deleted task %lu: %s", pt->id, pt->name);
    if (pt->number_of_notes > 0) {
        sprintf(str + strlen(str), " with %lu note%s", pt->number_of_notes, (pt->number_of_notes == 1 ? "" : "s"));
    }
    puts(str);
    pit_action(pt->id, "task", str);
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

void pit_task(char *argv[])
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
            case 'e': /* pit task -e [number] [-n name] [-s status] [-p priority] [-d date] [-t time]*/
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
}

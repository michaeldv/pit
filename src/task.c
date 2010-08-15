#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"
    
static int task_find_current(int id, PTask *ppt)
{
    if (id) {
        *ppt = (PTask)pit_table_find(tasks, id);
        if (!*ppt) die("could not find task %d", id);
    } else {
        *ppt = (PTask)pit_table_current(tasks);
        if (!*ppt) die("could not find current task");
    }
    return *ppt ? (*(PTask *)ppt)->id : 0;
}

static void task_log_create(PTask pt, POptions po)
{
    Action a = { pt->project_id, pt->id, 0 };

    sprintf(a.message, "created task %d: %s (status: %s, priority: %s", pt->id, po->task.name, po->task.status, po->task.priority);
    if (po->task.date > 0) sprintf(a.message + strlen(a.message), ", date: %s", format_date(po->task.date));
    if (po->task.time > 0) sprintf(a.message + strlen(a.message), ", time: %s", format_time(po->task.time));
    sprintf(a.message + strlen(a.message), ", project: %d)", pt->project_id);
    pit_action(&a);
}

static void task_log_update(PTask pt, POptions po)
{
    Action a = { pt->project_id, pt->id, 0 };
    bool empty = TRUE;

    sprintf(a.message, "updated task %d:", pt->id);
    if (po->task.name) {
        sprintf(a.message + strlen(a.message), " (name: %s", po->task.name);
        empty = FALSE;
    } else {
        sprintf(a.message + strlen(a.message), " %s (", pt->name);
    }
    if (po->task.status) {
        sprintf(a.message + strlen(a.message), "%sstatus: %s", (empty ? "" : ", "), po->task.status);
        empty = FALSE;
    }
    if (po->task.priority) {
        sprintf(a.message + strlen(a.message), "%spriority: %s", (empty ? "" : ", "), po->task.priority);
        empty = FALSE;
    }
    if (po->task.date) {
        if (po->task.date < 0) {
            sprintf(a.message + strlen(a.message), "%sdate: none", (empty ? "" : ", "));
        } else {
            sprintf(a.message + strlen(a.message), "%sdate: %s", (empty ? "" : ", "), format_date(po->task.date));
        }
        empty = FALSE;
    }
    if (po->task.time) {
        if (po->task.time < 0) {
            sprintf(a.message + strlen(a.message), "%stime: none", (empty ? "" : ", "));
        } else {
            sprintf(a.message + strlen(a.message), "%stime: %s", (empty ? "" : ", "), format_time(po->task.time));
        }
        empty = FALSE;
    }
    strcat(a.message, ")");
    pit_action(&a);
}

static void task_log_delete(int project_id, int id, char *name, int number_of_notes)
{
    Action a = { project_id, id, 0 };

    sprintf(a.message, "deleted task %d: %s", id, name);
    if (number_of_notes > 0) {
        sprintf(a.message + strlen(a.message), " with %d note%s", number_of_notes, (number_of_notes == 1 ? "" : "s"));
    }
    sprintf(a.message + strlen(a.message), " (project: %d)", project_id);
    pit_action(&a);
}

static void task_show(int id)
{
    PTask pt;

    pit_db_load();
    id = task_find_current(id, &pt);

    if (pt) {
        /* printf("The task was created on %s, last updated on %s\n", format_timestamp(pt->created_at), format_timestamp(pt->updated_at)); */
        printf("* %d: (%s) %s (project: %d, status: %s, priority: %s", pt->id, pt->username, pt->name, pt->project_id, pt->status, pt->priority);
        if (pt->date) printf(", date: %s", format_date(pt->date));
        if (pt->time) printf(", time: %s", format_time(pt->time));
        printf(", %d note%s)\n", pt->number_of_notes, pt->number_of_notes != 1 ? "s" : "");
        pit_table_mark(tasks, pt->id);
        if (pt->number_of_notes > 0)
            pit_note_list(pt);
        pit_db_save();
    } else {
        die("could not find the task");
    }
}

static void task_create(POptions po)
{
    pit_db_load();
    PProject pp = (PProject)pit_table_current(projects);

    if (!pp) {
        die("no project selected");
    } else {
        Task t = { 0 }, *pt;

        t.project_id = pp->id;
        if (!po->task.status) po->task.status = "open";
        if (!po->task.priority) po->task.priority = "normal";
        strncpy(t.name,     po->task.name,     sizeof(t.name)     - 1);
        strncpy(t.status,   po->task.status,   sizeof(t.status)   - 1);
        strncpy(t.priority, po->task.priority, sizeof(t.priority) - 1);
        strncpy(t.username, current_user(),    sizeof(t.username) - 1);
        t.date = max(0, po->task.date);
        t.time = max(0, po->task.time);

        pt = (PTask)pit_table_insert(tasks, (char *)&t);
        pit_table_mark(tasks, pt->id);
        pp->number_of_tasks++;
        task_log_create(pt, po);
        pit_db_save();
    }
}

static void task_update(int id, POptions po)
{
    PTask pt;

    pit_db_load();
    id = task_find_current(id, &pt);

    if (po->task.name)     strncpy(pt->name,     po->task.name,     sizeof(pt->name) - 1);
    if (po->task.status)   strncpy(pt->status,   po->task.status,   sizeof(pt->status) - 1);
    if (po->task.priority) strncpy(pt->priority, po->task.priority, sizeof(pt->priority) - 1);
    if (po->task.date) pt->date = max(0, po->task.date);
    if (po->task.time) pt->time = max(0, po->task.time);
    strncpy(pt->username, current_user(), sizeof(pt->username) - 1);
    pit_table_mark(tasks, pt->id);

    task_log_update(pt, po);
    pit_db_save();
}

static void task_parse_options(char **arg, POptions po)
{
    while(*++arg) {
        switch(pit_arg_option(arg)) {
        case 'n':
            po->task.name = pit_arg_string(++arg, "task name");
            break;
        case 's':
            po->task.status = pit_arg_string(++arg, "task status");
            break;
        case 'p':
            po->task.priority = pit_arg_string(++arg, "task priority");
            break;
        case 'd':
            po->task.date = pit_arg_date(++arg, "task date");
            break;
        case 't':
            po->task.time = pit_arg_time(++arg, "task time");
            break;
        default:
            die("invalid task option: %s", *arg);
        }
    }
}

void pit_task_list(POptions po, PProject pp)
{
    if (!tasks) pit_db_load();

    if (tasks->number_of_records > 0) {
        PPager ppager = pit_pager_initialize(PAGER_TASK, (pp ? 4 : 0), tasks->number_of_records);
        if (!pp) pp = (PProject)pit_table_current(projects);

        for_each_task(pt) {
            if (pp && pt->project_id != pp->id)
                continue;
            pit_pager_print(ppager, (char *)pt);
        }
        pit_pager_flush(ppager);
    }
}

/*
** A task could be deleted as standalone entity or as part of cascading project
** delete. In later case we're going to have 'pp' set and the database loaded.
*/
void pit_task_delete(int id, PProject pp)
{
    PTask pt;
    bool standalone = (pp == NULL);

    if (standalone) pit_db_load();
    id = task_find_current(id, &pt);
    if (standalone) pp = (PProject)pit_table_find(projects, pt->project_id);

    if (pp) {
        /*
        ** Preserve task name and number_of_notes before deleting the task since
        ** we need these for logging.
        */
        char *deleted_name = str2str(pt->name);
        int deleted_number_of_notes = pt->number_of_notes;
        /*
        ** Delete task notes if any.
        */
        if (pt->number_of_notes > 0) {
            for_each_note(pn) {
                if (pn->task_id == id) {
                    pit_note_delete(pn->id, pt);
                    --pn; /* Make the note pointer stay since it now points to the next note. */
                }
            }
        }
        pt = (PTask)pit_table_delete(tasks, id);
        if (pt) {
            pit_table_mark(tasks, 0); /* TODO: find better current task candidate.  */
            task_log_delete(pp->id, id, deleted_name, deleted_number_of_notes);
            if (standalone) {
                pp->number_of_tasks--;
                pit_db_save();
            }
            free(deleted_name);
        } else {
            free(deleted_name);
            die("could not delete task %d", id);
        }
    } else {
        die("could not find project for task %d", id);
    }
}

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
**   pit task -q [number | [-n name] [-s status] [-p priority] [-d date] [-t time]]
*/
void pit_task(char *argv[])
{
    int number = 0;
    char **arg = &argv[1];
    Options opt = {{ 0 }};

    if (!*arg) {
        pit_task_list(&opt, NULL); /* List all tasks for current project. */
    } else { /* pit task [number] */
        number = pit_arg_number(arg, NULL);
        if (number) {
            task_show(number);
        } else {
            switch(pit_arg_option(arg)) {
            case 'c': /* pit task -c name [-s status] [-p priority] [-d date] [-t time] */
                opt.task.name = pit_arg_string(++arg, "task name");
                task_parse_options(arg, &opt);
                task_create(&opt);
                break;
            case 'e': /* pit task -e [number] [-n name] [-s status] [-p priority] [-d date] [-t time] */
                number = pit_arg_number(++arg, NULL);
                if (!number) --arg;
                task_parse_options(arg, &opt);
                if (is_zero((char *)&opt.task, sizeof(opt.task))) {
                    die("nothing to update");
                } else {
                    task_update(number, &opt);
                }
                break;
            case 'd': /* pit task -d [number] */
                number = pit_arg_number(++arg, NULL);
                pit_task_delete(number, NULL); /* Delete the task, but keep its project. */
                break;
            case 'q': /* pit task -q [number | [-n name] [-s status] [-p priority] [-d date] [-t time]] */
                opt.task.id = pit_arg_number(++arg, NULL);
                if (opt.task.id) {
                    task_show(opt.task.id);
                } else {
                    task_parse_options(--arg, &opt);
                    if (is_zero((char *)&opt.task, sizeof(opt.task))) {
                        task_show(0); /* Show current task if any. */
                    } else {
                        pit_task_list(&opt, NULL);
                    }
                }
                break;
            default:
                die("invalid task option: %s", *arg);
            }
        }
    }
}

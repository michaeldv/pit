#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

#define for_each_entry(ppager, entry) for (entry = (char **)ppager->entries; (char *)*entry; entry++)
#define TASK(attr)    (((PTask)*pentry)->attr)
#define PROJECT(attr) (((PProject)*pentry)->attr)
#define ACTION(attr)  (((PAction)*pentry)->attr)

static void print_actions(PPager ppager)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%s (%%-%ds): %%s\n", ppager->max.action.username);
    for_each_entry(ppager, pentry) {
        printf(format, format_date(ACTION(created_at)), ACTION(username), ACTION(message));
    }
}

static void print_projects(PPager ppager)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%c %%%dd: (%%-%ds) [%%-%ds] %%-%ds (%%d task%%s)\n",
        ppager->max.project.id, ppager->max.project.username, ppager->max.project.status, ppager->max.project.name
    );
    for_each_entry(ppager, pentry) {
        printf(format,
            (PROJECT(id) == projects->current ? '*' : ' '),
            PROJECT(id),
            PROJECT(username),
            PROJECT(status),
            PROJECT(name),
            PROJECT(number_of_tasks),
            (PROJECT(number_of_tasks) != 1 ? "s" : "")
        );
    }
}

static void print_tasks(PPager ppager)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%c %%%dd: (%%-%ds) [%%-%ds] [%%-%ds] %%-%ds (%%d note%%s)\n",
        ppager->max.task.id, ppager->max.task.username, ppager->max.task.status, ppager->max.task.priority, ppager->max.task.name
    );
    for_each_entry(ppager, pentry) {
        printf(format, 
            (TASK(id) == tasks->current ? '*' : ' '),
            TASK(id),
            TASK(username),
            TASK(status),
            TASK(priority),
            TASK(name),
            TASK(number_of_notes),
            (TASK(number_of_notes) != 1 ? "s" : "")
        );
    }
}

static void print_tasks_with_date(PPager ppager)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%c %%%dd: (%%-%ds) [%%-%ds] [%%-%ds] %%-%ds %%-%ds (%%d note%%s)\n",
        ppager->max.task.id, ppager->max.task.username, ppager->max.task.status, ppager->max.task.priority, ppager->max.task.date, ppager->max.task.name
    );
    for_each_entry(ppager, pentry) {
        printf(format, 
            (TASK(id) == tasks->current ? '*' : ' '),
            TASK(id),
            TASK(username),
            TASK(status),
            TASK(priority),
            (TASK(date) ? format_date(TASK(date)) : ""),
            TASK(name),
            TASK(number_of_notes),
            (TASK(number_of_notes) != 1 ? "s" : "")
        );
    }
}

static void print_tasks_with_time(PPager ppager)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%c %%%dd: (%%-%ds) [%%-%ds] [%%-%ds] %%%ds %%-%ds (%%d note%%s)\n",
        ppager->max.task.id, ppager->max.task.username, ppager->max.task.status, ppager->max.task.priority, ppager->max.task.time, ppager->max.task.name
    );
    for_each_entry(ppager, pentry) {
        printf(format, 
            (TASK(id) == tasks->current ? '*' : ' '),
            TASK(id),
            TASK(username),
            TASK(status),
            TASK(priority),
            (TASK(time) ? format_time(TASK(time)) : ""),
            TASK(name),
            TASK(number_of_notes),
            (TASK(number_of_notes) != 1 ? "s" : "")
        );
    }
}

static void print_tasks_with_date_and_time(PPager ppager)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%c %%%dd: (%%-%ds) [%%-%ds] [%%-%ds] %%-%ds %%%ds %%-%ds (%%d note%%s)\n",
        ppager->max.task.id, ppager->max.task.username, ppager->max.task.status, ppager->max.task.priority, ppager->max.task.date, ppager->max.task.time, ppager->max.task.name
    );
    for_each_entry(ppager, pentry) {
        printf(format, 
            (TASK(id) == tasks->current ? '*' : ' '),
            TASK(id),
            TASK(username),
            TASK(status),
            TASK(priority),
            (TASK(date) ? format_date(TASK(date)) : ""),
            (TASK(time) ? format_time(TASK(time)) : ""),
            TASK(name),
            TASK(number_of_notes),
            (TASK(number_of_notes) != 1 ? "s" : "")
        );
    }
}

PPager pit_pager_initialize(int type, int number_of_entries)
{
    PPager ppager = calloc(1, sizeof(Pager));

    memset(ppager, 0, sizeof(Pager));
    ppager->type = type;
    ppager->entries = calloc(number_of_entries + 1, sizeof(char *));

    return ppager;
}

void pit_pager_print(PPager ppager, char *entry)
{
    char str[32];

    char **pentry = (char **)ppager->entries + ppager->number_of_entries++;
    *pentry = entry;

    for_each_entry(ppager, pentry) {
        switch(ppager->type) {
        case PAGER_TASK:
            sprintf(str, "%d", TASK(id));
            ppager->max.task.id = max(ppager->max.task.id, strlen(str));
            ppager->max.task.username = max(ppager->max.task.username, strlen(TASK(username)));
            ppager->max.task.name = max(ppager->max.task.name, strlen(TASK(name)));
            ppager->max.task.status = max(ppager->max.task.status, strlen(TASK(status)));
            ppager->max.task.priority = max(ppager->max.task.priority, strlen(TASK(priority)));
            if (TASK(date)) {
                ppager->max.task.date = max(ppager->max.task.date, strlen(format_date(TASK(date))));
            }
            if (TASK(time)) {
                ppager->max.task.time = max(ppager->max.task.time, strlen(format_time(TASK(time))));
            }
            break;
        case PAGER_PROJECT:
            sprintf(str, "%d", ((PProject)*pentry)->id);
            ppager->max.project.id = max(ppager->max.project.id, strlen(str));
            ppager->max.project.username = max(ppager->max.project.username, strlen(((PProject)*pentry)->username));
            ppager->max.project.name = max(ppager->max.project.name, strlen(((PProject)*pentry)->name));
            ppager->max.project.status = max(ppager->max.project.status, strlen(((PProject)*pentry)->status));
            break;
        case PAGER_ACTION:
            ppager->max.action.username = max(ppager->max.action.username, strlen(((PAction)*pentry)->username));
            ppager->max.action.subject = max(ppager->max.action.subject, strlen(((PAction)*pentry)->subject));
            break;
        default:
            die("invalid pager type: %d\n", ppager->type);
        }
    }
}

void pit_pager_flush(PPager ppager)
{
    switch(ppager->type) {
    case PAGER_TASK:
        if (!ppager->max.task.date && !ppager->max.task.time) {
            print_tasks(ppager);                         /* Neither date nor time. */
        } else if (ppager->max.task.date) {
            if (ppager->max.task.time) {
                print_tasks_with_date_and_time(ppager);  /* Both date and time. */
            } else {
                print_tasks_with_date(ppager);           /* Date but no time. */
            }
        } else {
            print_tasks_with_time(ppager);               /* Time but no date. */
        }
        break;

    case PAGER_PROJECT:
        print_projects(ppager);
        break;

    case PAGER_ACTION:
        print_actions(ppager);
        break;

    default:
        pit_pager_free(ppager);
        die("invalid pager type: %d\n", ppager->type);
    }
    pit_pager_free(ppager);
}

void pit_pager_free(PPager ppager)
{
    free(ppager->entries);
    free(ppager);
}

#undef TASK
#undef PROJECT
#undef ACTION

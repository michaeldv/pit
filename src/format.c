/*
** Copyright (c) 2010 Michael Dvorkin
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the Simplified BSD License (also
** known as the "2-Clause License" or "FreeBSD License".)
**
** This program is distributed in the hope that it will be useful,
** but without any warranty; without even the implied warranty of
** merchantability or fitness for a particular purpose.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

#define for_each_entry(pf, entry) for (entry = (char **)pf->entries; (char *)*entry; entry++)
#define TASK(attr)    (((PTask)*pentry)->attr)
#define PROJECT(attr) (((PProject)*pentry)->attr)
#define ACTION(attr)  (((PAction)*pentry)->attr)
#define NOTE(attr)    (((PNote)*pentry)->attr)

static void print_notes(PFormat pf)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%%dc %%%dd: (%%-%ds) %%s\n", pf->indent, pf->max.note.id, pf->max.note.username);
    for_each_entry(pf, pentry) {
        printf(format, (NOTE(id) == notes->current ? '*' : ' '), NOTE(id), NOTE(username), NOTE(message));
    }
}

static void print_actions(PFormat pf)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%s (%%-%ds): %%s\n", pf->max.action.username);
    for_each_entry(pf, pentry) {
        printf(format, format_date(ACTION(created_at)), ACTION(username), ACTION(message));
    }
}

static void print_projects(PFormat pf)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%c %%%dd: (%%-%ds) |%%-%ds| %%-%ds (%%d task%%s)\n",
        pf->max.project.id, pf->max.project.username, pf->max.project.status, pf->max.project.name
    );
    for_each_entry(pf, pentry) {
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

static void print_tasks(PFormat pf)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%%dc %%%dd: (%%-%ds) |%%-%ds| |%%-%ds| %%-%ds (%%d note%%s)\n",
        pf-> indent, pf->max.task.id, pf->max.task.username, pf->max.task.status, pf->max.task.priority, pf->max.task.name
    );
    for_each_entry(pf, pentry) {
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

static void print_tasks_with_date(PFormat pf)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%%dc %%%dd: (%%-%ds) |%%-%ds| |%%-%ds| %%-%ds %%-%ds (%%d note%%s)\n",
        pf->indent, pf->max.task.id, pf->max.task.username, pf->max.task.status, pf->max.task.priority, pf->max.task.date, pf->max.task.name
    );
    for_each_entry(pf, pentry) {
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

static void print_tasks_with_time(PFormat pf)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%%dc %%%dd: (%%-%ds) |%%-%ds| |%%-%ds| %%%ds %%-%ds (%%d note%%s)\n",
        pf->indent, pf->max.task.id, pf->max.task.username, pf->max.task.status, pf->max.task.priority, pf->max.task.time, pf->max.task.name
    );
    for_each_entry(pf, pentry) {
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

static void print_tasks_with_date_and_time(PFormat pf)
{
    char **pentry;
    char format[64];

    sprintf(format, "%%%dc %%%dd: (%%-%ds) |%%-%ds| |%%-%ds| %%-%ds %%%ds %%-%ds (%%d note%%s)\n",
        pf->indent, pf->max.task.id, pf->max.task.username, pf->max.task.status, pf->max.task.priority, pf->max.task.date, pf->max.task.time, pf->max.task.name
    );
    for_each_entry(pf, pentry) {
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

PFormat pit_format_initialize(int type, int indent, int number_of_entries)
{
    PFormat pf = calloc(1, sizeof(Format));

    memset(pf, 0, sizeof(Format));
    pf->type = type;
    pf->indent = indent;
    pf->entries = calloc(number_of_entries + 1, sizeof(char *));

    return pf;
}

void pit_format(PFormat pf, char *entry)
{
    char str[32];

    char **pentry = (char **)pf->entries + pf->number_of_entries++;
    *pentry = entry;

    for_each_entry(pf, pentry) {
        switch(pf->type) {
        case FORMAT_PROJECT:
            sprintf(str, "%d", PROJECT(id));
            pf->max.project.id = max(pf->max.project.id, strlen(str));
            pf->max.project.username = max(pf->max.project.username, strlen(PROJECT(username)));
            pf->max.project.name = max(pf->max.project.name, strlen(PROJECT(name)));
            pf->max.project.status = max(pf->max.project.status, strlen(PROJECT(status)));
            break;
        case FORMAT_TASK:
            sprintf(str, "%d", TASK(id));
            pf->max.task.id = max(pf->max.task.id, strlen(str));
            pf->max.task.username = max(pf->max.task.username, strlen(TASK(username)));
            pf->max.task.name = max(pf->max.task.name, strlen(TASK(name)));
            pf->max.task.status = max(pf->max.task.status, strlen(TASK(status)));
            pf->max.task.priority = max(pf->max.task.priority, strlen(TASK(priority)));
            if (TASK(date)) {
                pf->max.task.date = max(pf->max.task.date, strlen(format_date(TASK(date))));
            }
            if (TASK(time)) {
                pf->max.task.time = max(pf->max.task.time, strlen(format_time(TASK(time))));
            }
            break;
        case FORMAT_NOTE:
            sprintf(str, "%d", NOTE(id));
            pf->max.note.id = max(pf->max.note.id, strlen(str));
            pf->max.note.username = max(pf->max.note.username, strlen(NOTE(username)));
            break;
        case FORMAT_ACTION:
            pf->max.action.username = max(pf->max.action.username, strlen(ACTION(username)));
            pf->max.action.message = max(pf->max.action.message, strlen(ACTION(message)));
            break;
        default:
            die("invalid format: %d\n", pf->type);
        }
    }
}

void pit_format_flush(PFormat pf)
{
    switch(pf->type) {
    case FORMAT_TASK:
        if (!pf->max.task.date && !pf->max.task.time) {
            print_tasks(pf);                         /* Neither date nor time. */
        } else if (pf->max.task.date) {
            if (pf->max.task.time) {
                print_tasks_with_date_and_time(pf);  /* Both date and time. */
            } else {
                print_tasks_with_date(pf);           /* Date but no time. */
            }
        } else {
            print_tasks_with_time(pf);               /* Time but no date. */
        }
        break;

    case FORMAT_PROJECT:
        print_projects(pf);
        break;

    case FORMAT_ACTION:
        print_actions(pf);
        break;

    case FORMAT_NOTE:
        print_notes(pf);
        break;

    default:
        pit_format_free(pf);
        die("invalid format: %d\n", pf->type);
    }
    pit_format_free(pf);
}

void pit_format_free(PFormat pf)
{
    free(pf->entries);
    free(pf);
}

#undef TASK
#undef PROJECT
#undef ACTION
#undef NOTE

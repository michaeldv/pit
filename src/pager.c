#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

#define for_each_entry(ppager, entry) for (entry = (uchar **)ppager->entries; (uchar *)*entry; entry++)

PPager pit_pager_initialize(ulong type, ulong number_of_entries)
{
    PPager ppager = calloc(1, sizeof(Pager));

    memset(ppager, 0, sizeof(Pager));
    ppager->type = type;
    ppager->entries = calloc(number_of_entries + 1, sizeof(uchar *));

    return ppager;
}

void pit_pager_print(PPager ppager, uchar *entry)
{
    char str[32];
    uchar **pentry = (uchar **)ppager->entries + ppager->number_of_entries++;
    *pentry = entry;

    for_each_entry(ppager, pentry) {
        switch(ppager->type) {
        case PAGER_TASK:
            sprintf(str, "%lu", ((PTask)*pentry)->id);
            ppager->max.task.id = max(ppager->max.task.id, strlen(str));
            ppager->max.task.name = max(ppager->max.task.name, strlen(((PTask)*pentry)->name));
            ppager->max.task.status = max(ppager->max.task.status, strlen(((PTask)*pentry)->status));
            ppager->max.task.priority = max(ppager->max.task.priority, strlen(((PTask)*pentry)->priority));
            break;
        case PAGER_PROJECT:
            sprintf(str, "%lu", ((PProject)*pentry)->id);
            ppager->max.project.id = max(ppager->max.project.id, strlen(str));
            ppager->max.project.name = max(ppager->max.project.name, strlen(((PProject)*pentry)->name));
            ppager->max.project.status = max(ppager->max.project.status, strlen(((PProject)*pentry)->status));
            break;
        default:
            die("invalid pager type: %d\n", ppager->type);
        }
    }
}

void pit_pager_flush(PPager ppager)
{
    uchar **pentry;
    char format[64];

    switch(ppager->type) {
    case PAGER_TASK:
        sprintf(format, "%%c %%%dlu: %%-%ds %%-%ds %%-%ds (%%lu note%%s)\n",
            ppager->max.task.id, ppager->max.task.status, ppager->max.task.priority, ppager->max.task.name
        );
        for_each_entry(ppager, pentry) {
            printf(format, 
                (((PTask)*pentry)->id == tasks->current ? '*' : ' '),
                ((PTask)*pentry)->id,
                ((PTask)*pentry)->status,
                ((PTask)*pentry)->priority,
                ((PTask)*pentry)->name,
                ((PTask)*pentry)->number_of_notes,
                (((PTask)*pentry)->number_of_notes != 1 ? "s" : "")
            );
        }
        break;
    case PAGER_PROJECT:
        sprintf(format, "%%c %%%dlu: %%-%ds %%-%ds (%%lu open, %%lu closed task%%s)\n",
            ppager->max.project.id, ppager->max.project.status, ppager->max.project.name
        );
        for_each_entry(ppager, pentry) {
            printf(format,
                (((PProject)*pentry)->id == projects->current ? '*' : ' '),
                ((PProject)*pentry)->id,
                ((PProject)*pentry)->status,
                ((PProject)*pentry)->name,
                ((PProject)*pentry)->number_of_open_tasks,
                ((PProject)*pentry)->number_of_closed_tasks,
                (projects->number_of_records != 1 ? "s" : "")
            );
        }
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

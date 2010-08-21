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

static void action_list()
{
    pit_db_load();
    if (actions->number_of_records > 0) {
        PFormat pf = pit_format_initialize(FORMAT_ACTION, 0, actions->number_of_records);
        for_each_action(pa) {
            pit_format(pf, (char *)pa);
        }
        pit_format_flush(pf);
    }
}

void pit_action(PAction pa)
{
    if (pa) {
        strncpy(pa->username, current_user(), sizeof(pa->username) - 1);
        pit_table_insert(actions, (char *)pa);
        if (pa->project_id || pa->task_id || pa->note_id) 
            puts(pa->message);
    }
}

void pit_log(char *argv[])
{
    action_list();
}
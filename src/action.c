#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static void action_list()
{
    pit_db_load();
    if (actions->number_of_records > 0) {
        PFormat pf = pit_pager_initialize(PAGER_ACTION, 0, actions->number_of_records);
        for_each_action(pa) {
            pit_pager_print(pf, (char *)pa);
        }
        pit_pager_flush(pf);
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
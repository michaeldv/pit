#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

static void action_list()
{
    PPager ppager;

    pit_db_load();
    if (actions->number_of_records > 0) {
        ppager = pit_pager_initialize(PAGER_ACTION, actions->number_of_records);
        for_each_action(pa) {
            pit_pager_print(ppager, (uchar *)pa);
        }
        pit_pager_flush(ppager);
    }
}

void pit_action(ulong id, char *subject, char *message)
{
    static Action action;

    memset(&action, 0, sizeof(action));

    action.subject_id = id;
    strncpy(action.subject, subject, sizeof(action.subject) - 1);
    strncpy(action.username, current_user(), sizeof(action.username) - 1);
    strncpy(action.message, message, sizeof(action.message) - 1);

    pit_table_insert(actions, (uchar *)&action);
}

void pit_log(char *argv[])
{
    action_list();
}